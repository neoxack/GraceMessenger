#pragma once

#include <thread>
#include <asio.hpp>
#include <iostream>
#include <unordered_map>

#include "node.h"
#include "routing_table.h"
#include "network_service.h"
#include "messages/packets.h"


namespace GraceMessenger
{
	namespace DHT
	{
		using namespace asio::ip;

		typedef std::function<void(const node_id *id, bool success, const udp::endpoint *result, int error)> find_node_callback;
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

			dht(const dht&) = delete;
			dht& operator=(const dht&) = delete;

			dht(const std::string &ip_adr, unsigned short port, const node_id &id)
			{
				_state = Off;
				udp::endpoint endpoint;
				address adr = address::from_string(ip_adr);
				endpoint = udp::endpoint(adr, port);

				_main_node.endpoint = endpoint;
				_main_node.id = id;

				start();
			}


			bool is_connected() const
			{
				return _state == Bootstrapped;
			}

		

			bool bootstrap(const std::string &str_id, const std::string &ip_adr, unsigned short port, const bootstrap_callback &callback)
			{
				udp::endpoint endpoint;
				asio::error_code ec;
				address adr = address::from_string(ip_adr, ec);
				if (ec.value() != 0)
				{
					udp::resolver resolver(_io_service);
					udp::resolver::query query(udp::v4(), ip_adr, "");
					udp::resolver::iterator iter = resolver.resolve(query);
					udp::endpoint ep = *iter;
					endpoint = udp::endpoint(ep.address(), port);
				}
				else
					endpoint = udp::endpoint(adr, port);
				auto id = id_from_string(str_id);

				if (_state == Started)
				{
					timer_start(FIND_PERIOD);
					_bootstrap_callback = callback;
					_routing_table->update(id, endpoint);
					get_nodes_packet get_nodes_pack(_main_node.id.data(), _main_node.id.data());
					send_message(&get_nodes_pack.header, endpoint);
					return true;
				}
				return false;
			}

			void find_node(const node_id &id, const find_node_callback &callback)
			{
				auto fnode = _routing_table->find_node(id);

				if (fnode != nullptr)
				{
					callback(&id, true, &fnode->endpoint, 0);
				}
				else
				{
					auto closest_nodes = _routing_table->find_closest_nodes(id, ALPHA);
					_found_nodes_callbacks.insert(std::make_pair(id, callback));
					for (auto &n : closest_nodes)
					{
						get_nodes_packet get_nodes_pack(_main_node.id.data(), id.data());
						send_message(&get_nodes_pack.header, n->endpoint);
					}
				}
			}

			size_t get_nodes_count() const
			{
				return _routing_table->get_nodes_count();
			}

			udp::endpoint endpoint() const
			{
				return _main_node.endpoint;
			}

			~dht()
			{
				_state = Off;
			};

		private:

			bool start()
			{
				if (_state == Off)
				{
					_network_service = std::make_unique<network_service>(_io_service, _main_node.endpoint, bind(&dht::handle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
					_routing_table = std::make_unique<routing_table>(_main_node, *_network_service.get());
					_state = Started;
					return true;
				}
				return false;
			}

			void handle_ping(const udp::endpoint &endpoint)
			{
				pong_packet pong_pack(_main_node.id.data());
				send_message(&pong_pack.header, endpoint);
			}

			void handle_pong(const pong_packet *pong_pack, const udp::endpoint &endpoint)
			{
				node_id id;
				memcpy(id.data(), pong_pack->header.sender, USER_ID_LENGTH);

				auto callback = _found_nodes_callbacks[id];
				if (callback != nullptr)
				{
					_found_nodes_callbacks.erase(id);
					callback(&id, true, &endpoint, 0);
				}
			}

			void handle_get_nodes(const get_nodes_packet *get_nodes_pack, const udp::endpoint &endpoint)
			{		
				node_id find_id;
				memcpy(find_id.data(), get_nodes_pack->requested_id, find_id.size());
				auto closest_nodes = _routing_table->find_closest_nodes(find_id, MAX_NODES_IN_RESPONSE);

				send_nodes_packet send_nodes_pack(_main_node.id.data(), closest_nodes);
				send_message(&send_nodes_pack.header, endpoint);
			}

			void handle_get_nodes_response(const send_nodes_packet *pack, const udp::endpoint &)
			{
				if (_state == Started)
				{
					_state = Bootstrapped;
					if (_bootstrap_callback != nullptr)
						_bootstrap_callback(true, 0);
				}
				if (_state == Bootstrapped)
				{
					for (size_t i = 0; i < pack->nodes_count; i++)
					{
						node_id id;
						memcpy(id.data(), pack->header.sender, USER_ID_LENGTH);
						if (id != _main_node.id)
						{
							ping_packet ping_pack(_main_node.id.data());
							send_message(&ping_pack.header, pack->nodes[i].endpoint);
						}
					}			
				}
			}

			void handle(const udp::endpoint &endpoint, const std::array<char, BUF_SIZE> & buffer, size_t bytes_recvd)
			{	
				std::cout << "handle() " << _main_node.endpoint.port() << " <- " << endpoint.port() << ", bytes received: " << bytes_recvd << std::endl;

				if (bytes_recvd >= sizeof(dht_header))
				{
					const dht_header *header = reinterpret_cast<const dht_header *>(buffer.data());

					switch (header->type)
					{
						case PING_REQUEST:
						{
							handle_ping(endpoint);
							break;
						}
						case PING_RESPONSE:
						{
							const pong_packet *pong_pack = reinterpret_cast<const pong_packet *>(header);
							handle_pong(pong_pack, endpoint);
							break;
						}
						case GET_NODES:
						{
							const get_nodes_packet *get_nodes_pack = reinterpret_cast<const get_nodes_packet *>(header);
							handle_get_nodes(get_nodes_pack, endpoint);
							break;
						}
						case SEND_NODES:
						{
							const send_nodes_packet *send_nodes_pack = reinterpret_cast<const send_nodes_packet *>(header);
							handle_get_nodes_response(send_nodes_pack, endpoint);
							break;
						}
					}
					node_id id;
					memcpy(id.data(), header->sender, USER_ID_LENGTH);
					_routing_table->update(id, endpoint);
				}
			}

			void send_message(const dht_header *message, const udp::endpoint &endpoint)
			{
				if (endpoint != _main_node.endpoint)
				{
					_network_service->send(message, endpoint);
				}
			}

			void timer_start(size_t interval)
			{
				std::thread([this, interval]()
				{
					while (_state != Off)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(interval));
						_network_service->async_run(std::bind(&dht::periodic_task, this));
					}
				}).detach();
			}

			void periodic_task()
			{
				auto good_node = _routing_table->get_random_good_node();
				if (good_node != nullptr)
				{
					get_nodes_packet get_nodes_pack(_main_node.id.data(), _main_node.id.data());
					send_message(&get_nodes_pack.header, good_node->endpoint);
				}
				_routing_table->ping_nodes();
			}

			dht_state _state;
			bootstrap_callback _bootstrap_callback;
			asio::io_service _io_service;
			std::unique_ptr<network_service> _network_service;
			std::unique_ptr<routing_table> _routing_table;
			node _main_node;
			std::unordered_map<node_id, find_node_callback> _found_nodes_callbacks;
		};
	}
}



