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
		dht(const std::string &ip_adr, unsigned short port, const node_id &id)
		{
			_state = Off;
			asio::error_code ec;
			udp::endpoint endpoint;
			address adr = address::from_string(ip_adr, ec);
			if (ec.value() != 0)
			{
				tcp::resolver resolver(_io_service);
				tcp::resolver::query query(tcp::v4(), ip_adr, "");
				tcp::resolver::iterator iter = resolver.resolve(query);
				tcp::endpoint ep = *iter;
				endpoint = udp::endpoint(ep.address(), port);
			}
			else
				endpoint = udp::endpoint(adr, port);
			
			_network_service = std::make_unique<network_service>(_io_service, endpoint, bind(&dht::handle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			_main_node.endpoint = endpoint;
			_main_node.id = id;
			_routing_table = std::make_unique<routing_table>(_main_node, *_network_service.get());
		}

		bool start()
		{
			if (_state == Off)
			{
				_network_service->start();
				
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
				timer_start(FIND_PERIOD);
				_bootstrap_callback = callback;
				_routing_table->update(node);
				Messages::find_node<Messages::Request> find_message(_main_node, _main_node.id, get_random(), TTL);
				send_message(find_message, node.endpoint);
				return true;
			}
			return false;
		}

		bool bootstrap(const std::string &str_id, const std::string &ip_adr, unsigned short port, const bootstrap_callback &callback)
		{
			node bootstrap_node;
			asio::error_code ec;
			address adr = address::from_string(ip_adr, ec);
			if (ec.value()!=0)
			{
				tcp::resolver resolver(_io_service);
				tcp::resolver::query query(tcp::v4(), ip_adr, "");
				tcp::resolver::iterator iter = resolver.resolve(query);
				tcp::endpoint ep = *iter;
				bootstrap_node.endpoint = udp::endpoint(ep.address(), port);
			}
			else
				bootstrap_node.endpoint = udp::endpoint(adr, port);
			bootstrap_node.id = node_id_from_string(str_id);
			return bootstrap(bootstrap_node, callback);
		}

		void find_node(const node_id &id, const find_node_callback &callback)
		{
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
					Messages::find_node<Messages::Request> find_message(_main_node, id, get_random(), TTL);
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
				if (_bootstrap_callback != nullptr)
					_bootstrap_callback(true, 0);
			}

			Messages::ping<false> ping_response(ping_message.header.transaction_id, ping_message.random);
			send_message(ping_response, endpoint);
		}

		void handle_find_node_request(const Messages::find_node<Messages::Request> &find_node_message, const udp::endpoint &endpoint)
		{	
			auto node = _routing_table->find_node(find_node_message.find_id);
			if (node != nullptr && find_node_message.header.sender_id != find_node_message.find_id)
			{
				Messages::find_node<Messages::Response> message(find_node_message.finder, find_node_message.find_id, find_node_message.header.transaction_id, *node);
				send_message(message, find_node_message.finder.endpoint);
			}
			else
			{
				if (find_node_message.ttl > 0)
				{
					auto closest_nodes = _routing_table->find_closest_nodes(find_node_message.find_id, ALPHA);
					for (auto &&c_node : closest_nodes)
					{
						Messages::find_node<Messages::Request> find_message(find_node_message.finder, find_node_message.find_id, find_node_message.header.transaction_id, find_node_message.ttl - 1);
						send_message(find_message, c_node->endpoint);
					}
				}
				_routing_table->ping(find_node_message.finder);
				
			}
			_routing_table->update(find_node_message.finder);						
		}

		void handle_find_node_response(const Messages::find_node<Messages::Response> &find_node_message, const udp::endpoint &endpoint)
		{
			if (_state == Started)
			{
				_state = Bootstrapped;
				if (_bootstrap_callback != nullptr)
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
			using namespace Messages;
			std::cout << std::endl << "handle() " << _main_node.endpoint.port() << " <- " << endpoint.port() << ", bytes received: " << bytes_recvd;
			message_header header;
			header.parse(buffer.data());
			node n;
			n.id = header.sender_id;
			n.endpoint = endpoint;
			
			switch (header.type)
			{
				case PING_REQUEST:
				{
					ping<true> ping_message;
					ping_message.parse(buffer, header);
					handle_ping_request(ping_message, endpoint);
				//	std::cout << " [ ping ]";
					break;
				}
				case PING_RESPONSE:
				{
					ping<false> ping_message;
					ping_message.parse(buffer, header);
					_sent_messages.erase(header.transaction_id);
				//	std::cout << " [ pong ]";
					break;
				}
				case FIND_NODE_REQUEST:
				{
					Messages::find_node<Request> find_node_message;
					find_node_message.parse(buffer, header);
					handle_find_node_request(find_node_message, endpoint);
				//	std::cout << " [ find request ]";
					break;
				}
				case FIND_NODE_RESPONSE:
				{
					Messages::find_node<Response> find_node_message;
					find_node_message.parse(buffer, header);
					handle_find_node_response(find_node_message, endpoint);
					_sent_messages.erase(header.transaction_id);
				//	std::cout << " [ find response ]";
					break;
				}
			}
			std::cout << std::endl;
			_routing_table->update(n);
			
		}

		template<typename Message>
		void send_message(Message &message, const udp::endpoint &endpoint)
		{
			if (endpoint != _main_node.endpoint)
			{
				message.header.sender_id = _main_node.id;
				_network_service->send(message, endpoint);
			}
			//_sent_messages[message.header.transaction_id] = message.header;
		}

		void timer_start(size_t interval)
		{
			std::thread([this, interval]()
			{
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(interval));
					_network_service->async_run(std::bind(&dht::periodic_task, this));
				}
			}).detach();
		}

		void periodic_task()
		{
			auto random_node = _routing_table->get_random_good_node();
			if (random_node != nullptr)
			{
				Messages::find_node<Messages::Request> find_message(_main_node, _main_node.id, get_random(), TTL);
				send_message(find_message, random_node->endpoint);
			}
			_routing_table->ping_nodes();
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



