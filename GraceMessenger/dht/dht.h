#pragma once

#include <thread>
#include <asio.hpp>
#include <iostream>
#include <unordered_map>

#include "node.h"
#include "utils.h"
#include "routing_table.h"
#include "network_service.h"
#include "messages/find_node.h"
#include "messages/ping.h"


namespace GraceDHT
{
	using namespace asio::ip;

	typedef std::function<void(const node_id *id, bool success, const node *result, int error)> find_node_callback;
	typedef std::function<void(bool success, int error)> bootstrap_callback;

	typedef enum dht_state
	{
		Off,
		Started,
		Bootstrapped
	} dht_state;

	class dht
	{
		
	public:
		dht(const std::string &ip_adr, unsigned short port)
		{
			_state = Off;
			address adr = address::from_string(ip_adr);
			udp::endpoint endpoint = udp::endpoint(adr, port);
			
			_network_service = std::make_unique<network_service>(_io_service, endpoint, bind(&dht::handle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			_main_node.endpoint = endpoint;
			fill_random_id(_main_node.id);
			_routing_table = std::make_unique<routing_table>(&_main_node);
		}

		bool start()
		{
			if (_state == Off)
			{
				_network_service->start();
				timer_start(std::bind(&dht::periodic_task, this), 60000);
				_state = Started;
				return true;
			}
			return false;
		}

		bool is_connected() const
		{
			return _state == Bootstrapped;
		}

		bool bootstrap(const node &node, const bootstrap_callback &callback)
		{
			if (_state == Started)
			{
				_bootstrap_callback = callback;
				_routing_table->update(node);
				Messages::find_node<true> find_message(_main_node, _main_node.id, get_random(), TTL);
				send_message(find_message, node.endpoint);
				return true;
			}
			return false;
		}

		bool bootstrap(const std::string &str_id, const std::string &ip_adr, unsigned short port, const bootstrap_callback &callback)
		{
			node bootstrap_node;
			address adr = address::from_string(ip_adr);
			bootstrap_node.id = node_id_from_string(str_id);
			bootstrap_node.endpoint = udp::endpoint(adr, port);
			return bootstrap(bootstrap_node, callback);
		}

		void find_node(const std::string &str_id, const find_node_callback &callback)
		{
			auto id = node_id_from_string(str_id);
			find_node(id, callback);
		}

		void find_node(const node_id &id, const find_node_callback &callback)
		{
			node result;
			auto fnode = _routing_table->find_node(id);
			
			if (fnode != nullptr)
			{
				callback(&id, true, fnode, 0);
			}
			else
			{
				auto closest_nodes = _routing_table->find_closest_nodes(id, ALPHA);				
				_found_nodes_callbacks.insert(make_pair(id, callback));
				for (auto &node : closest_nodes)
				{
					Messages::find_node<true> find_message(_main_node, id, get_random(), TTL);
					send_message(find_message, node->endpoint);
				}			
			}
		}

		size_t get_nodes_count() const
		{
			return _routing_table->get_nodes_count();
		}

		std::string get_node_id() const
		{
			return node_id_to_string(_main_node.id);
		}

		const node* get_node() const
		{
			return &_main_node;
		}

		~dht(){};

	private:

		void handle_ping_request(const Messages::ping<true> &ping_message, const udp::endpoint &endpoint)
		{
			if (_state == Started)
			{
				_state = Bootstrapped;
				_bootstrap_callback(true, 0);
			}
			Messages::ping<false> ping_response(ping_message.header.transaction_id, ping_message.random);
			send_message(ping_response, endpoint);
		}

		void handle_find_node_request(const Messages::find_node<true> &find_node_message, const udp::endpoint &endpoint)
		{	
			auto node = _routing_table->find_node(find_node_message.find_id);
			if (node != nullptr && find_node_message.header.sender_id != find_node_message.find_id)
			{
				Messages::find_node<false> message(find_node_message.finder, find_node_message.find_id, find_node_message.header.transaction_id, *node);
				send_message(message, find_node_message.finder.endpoint);
			}
			else
			{
				if (find_node_message.ttl > 0)
				{
					auto closest_nodes = _routing_table->find_closest_nodes(find_node_message.find_id, ALPHA);
					for (auto &node : closest_nodes)
					{
						Messages::find_node<true> find_message(find_node_message.finder, find_node_message.find_id, find_node_message.header.transaction_id, find_node_message.ttl - 1);
						send_message(find_message, node->endpoint);
					}
				}
				ping(find_node_message.finder);
				
			}
			_routing_table->update(find_node_message.finder);						
		}

		void handle_find_node_response(const Messages::find_node<false> &find_node_message, const udp::endpoint &endpoint)
		{
			if (_state == Started)
			{
				_state = Bootstrapped;
				_bootstrap_callback(true, 0);
			}
			if (_state == Bootstrapped)
			{
				auto callback = _found_nodes_callbacks[find_node_message.find_id];
				if (callback != nullptr)
				{
					_found_nodes_callbacks.erase(find_node_message.find_id);
					callback(&find_node_message.find_id, true, &find_node_message.found_node, 0);	
				}
			}		
		}

		void handle(const udp::endpoint & endpoint, const std::array<char, BUF_SIZE> & buffer, size_t bytes_recvd)
		{
			std::cout << "handle() " << _main_node.endpoint.port() << " <- " << endpoint.port() << ", bytes received: " << bytes_recvd << std::endl;
			Messages::message_header header;
			header.parse(buffer.data());
			node n;
			n.id = header.sender_id;
			n.endpoint = endpoint;
			
			switch (header.type)
			{
				case Messages::PING_REQUEST:
				{
					Messages::ping<true> ping_message;
					ping_message.parse(buffer, header);
					handle_ping_request(ping_message, endpoint);
					break;
				}
				case Messages::PING_RESPONSE:
				{
					Messages::ping<false> ping_message;
					ping_message.parse(buffer, header);
					_sent_messages.erase(header.transaction_id);
					break;
				}
				case Messages::FIND_NODE_REQUEST:
				{
					Messages::find_node<true> find_node_message;
					find_node_message.parse(buffer, header);
					handle_find_node_request(find_node_message, endpoint);
					break;
				}
				case Messages::FIND_NODE_RESPONSE:
				{
					Messages::find_node<false> find_node_message;
					find_node_message.parse(buffer, header);
					handle_find_node_response(find_node_message, endpoint);
					_sent_messages.erase(header.transaction_id);
					break;
				}
			}
			_routing_table->update(n);
			
		}

		template<typename Message>
		void send_message(Message &message, const udp::endpoint &endpoint)
		{
			message.header.sender_id = _main_node.id;
			_network_service->send(message, endpoint);		
			//_sent_messages[message.header.transaction_id] = message.header;
		}

		void ping(const node &node)
		{
			Messages::ping<true> ping_message(get_random(), get_random());
			send_message(ping_message, node.endpoint);
		}

		void timer_start(std::function<void(void)> func, size_t interval)
		{
			std::thread([func, interval]() {
				while (true)
				{
					func();
					std::this_thread::sleep_for(std::chrono::milliseconds(interval));
				}
			}).detach();
		}

		void periodic_task()
		{
			
		}

		dht_state _state;
		bootstrap_callback _bootstrap_callback;
		asio::io_service _io_service;
		std::unique_ptr<network_service> _network_service;
		std::unique_ptr<routing_table> _routing_table;
		node _main_node;
		std::unordered_map<uint32_t, Messages::message_header> _sent_messages;
		std::unordered_map<node_id, find_node_callback> _found_nodes_callbacks;
	};
}


