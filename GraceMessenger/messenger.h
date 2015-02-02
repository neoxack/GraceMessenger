#pragma once

#include <memory>

#include "status.h"
#include "message.h"
#include "callbacks.h"
#include "network/network_service.h"
#include "config.h"
#include "contact.h"
#include "friend_request.h"
#include "network/dht/dht.h"
#include "network/nat_pmp.h"

namespace GraceMessenger
{

	enum network_status
	{
		Offline = 0,
		Online = 1
	};

	class messenger
	{
	public:

		messenger(const messenger&) = delete;
		messenger& operator=(const messenger&) = delete;

		messenger(const config &config, const callbacks &callbacks) :
			_callbacks(callbacks),
			_config(config)
		{	
			_nat_pmp.forward_port(_config.dht_port, _config.dht_port);
			_nat_pmp.forward_port(_config.dht_port+1, _config.dht_port+1);
			_dht = std::make_unique<DHT::dht>(_config.dht_port, _config.user.id);
		}

		bool dht_bootstrap(const std::string &str_id, const std::string &ip_adr, unsigned short port)
		{
			asio::ip::udp::endpoint endpoint(_dht->endpoint().address(), _config.dht_port + 1);;
			_network_service = std::make_unique<Network::network_service>(_io_service, endpoint, bind(&messenger::handle_packets, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			return _dht->bootstrap(str_id, ip_adr, port, _callbacks.dht_bootstrapped);
		}

		status online()
		{
			if (_network_status == Offline)
			{
				_network_status = Online;

			}
			return status();
		}

		status offline()
		{
			if (_network_status == Online)
			{
				_network_status = Offline;

			}
			return status();
		}

		void send_message(const message &m)
		{
			//_messages.emplace(m.id, m);

			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			simple_text_message_packet simple_text_message_pack(m.id, m.content);

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), m.contact.data());
			crypted_pack.crypt(simple_text_message_pack, shared_k);

			_dht->find_node(m.contact,
				[this, crypted_pack, m](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
				{
					asio::ip::udp::endpoint endpoint(result->address(), result->port() + 1);
					_network_service->send(crypted_pack, endpoint);
					_callbacks.message_sent(&m);
				}				
				else
				{
					status stat;
					stat.message = "dht not found node";
					stat.code = 1;
					LOG(Error, "dht not found node");
					_callbacks.message_send_error(&m, &stat);
				}
			});
		}

		void send_friend_request(const friend_request &request)
		{
			_friend_requests.emplace(request.id, request);

			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			friend_request_packet friend_request_pack(request.id, request.content);

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), request.contact.data());
			crypted_pack.crypt(friend_request_pack, shared_k);

			_dht->find_node(request.contact,
				[this, crypted_pack, request](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
				{
					asio::ip::udp::endpoint endpoint(result->address(), result->port() + 1);
					_network_service->send(crypted_pack, endpoint);
					_callbacks.friend_request_sended(&request);
				}
				else
				{
					status stat;
					stat.message = "dht not found node";
					stat.code = 1;
					LOG(Error, "dht not found node");
					_callbacks.friend_request_send_error(&request, &stat);
				}
			});
		}

		std::string id() const
		{
			return id_to_string(_config.user.id);
		}

		uint16_t port() const
		{
			return dht_port() + 1;
		}

		uint16_t dht_port() const
		{
			return _config.dht_port;
		}

		uint16_t dht_nodes_count() const
		{
			return _dht->get_nodes_count();
		}
		

	private:

		void send_add_friend_packet(const user_id &user_id)
		{
			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			add_friend_packet add_friend_pack(get_random());

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), user_id.data());
			crypted_pack.crypt(add_friend_pack, shared_k);

			_dht->find_node(user_id,
				[this, crypted_pack](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
					_network_service->send(crypted_pack, *result);
				else
					LOG(Error, "dht not found node");
			});
		}

		void send_delete_friend_packet(const user_id &user_id)
		{
			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			delete_friend_packet delete_friend_pack(get_random());

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), user_id.data());
			crypted_pack.crypt(delete_friend_pack, shared_k);

			_dht->find_node(user_id,
				[this, crypted_pack](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
					_network_service->send(crypted_pack, *result);
				else
					LOG(Error, "dht not found node");
			});

		}


		void handle_packets(const asio::ip::udp::endpoint& endpoint, const std::array<uint8_t, Network::BUF_SIZE>& data, size_t size)
		{
			using namespace Network;
			using namespace Crypto;

			crypted_packet crypted_pack;
			crypted_pack.parse(data.data(), size);

			
			shared_key shared_k;
			public_key sender_id;
			memcpy(sender_id.data(), crypted_pack.header.sender, KEY_LENGTH);

			generate_shared_key(shared_k.data(), _config.user.private_key.data(), sender_id.data());

			data_packet_header *data_packet_header_ptr = crypted_pack.decrypt(shared_k);

			switch (data_packet_header_ptr->type)
			{
				case ChangeStatus:
				{
					change_status_packet *change_status_pack = reinterpret_cast<change_status_packet *>(data_packet_header_ptr);
					break;
				}
				case FriendRequest:
				{
					friend_request_packet *friend_request_pack = reinterpret_cast<friend_request_packet *>(data_packet_header_ptr);
					break;
				}
				case AddFriend:
				{
					add_friend_packet *add_friend_pack = reinterpret_cast<add_friend_packet *>(data_packet_header_ptr);
					break;
				}
				case DeleteFriend:
				{
					delete_friend_packet *delete_friend_pack = reinterpret_cast<delete_friend_packet *>(data_packet_header_ptr);
					break;
				}
				case SimpleTextMessage:
				{
					simple_text_message_packet *simple_text_message_pack = reinterpret_cast<simple_text_message_packet *>(data_packet_header_ptr);
					message m;
					m.id = simple_text_message_pack->id;
					m.contact = sender_id;
					m.content = std::wstring(simple_text_message_pack->text, simple_text_message_pack->text_length);
					m.input = true;
					m.timestamp = get_timestamp();
					//_messages.emplace(m.id, m);
					_callbacks.message_received(&m);
					break;
				}
				case Ping:
				{
					ping_packet *ping_pack = reinterpret_cast<ping_packet *>(data_packet_header_ptr);
					pong(sender_id);
					break;
				}
				case Pong:
				{
					pong_packet *pong_pack = reinterpret_cast<pong_packet *>(data_packet_header_ptr);
					break;
				}
			}
		}


		void ping(const user_id &user_id)
		{
			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			ping_packet ping_pack;

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), user_id.data());
			crypted_pack.crypt(ping_pack, shared_k);

			_dht->find_node(user_id,
				[this, crypted_pack](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
					_network_service->send(crypted_pack, *result);
				else
					LOG(Error, "dht not found node");
			});
		}

		void pong(const user_id &user_id)
		{
			using namespace Network;
			using namespace Crypto;
			crypted_packet crypted_pack(_config.user.id.data());
			pong_packet pong_pack;

			shared_key shared_k;
			generate_shared_key(shared_k.data(), _config.user.private_key.data(), user_id.data());
			crypted_pack.crypt(pong_pack, shared_k);

			_dht->find_node(user_id,
				[this, crypted_pack](const DHT::node_id *id, bool success, const asio::ip::udp::endpoint* result, int error)
			{
				if (success)
					_network_service->send(crypted_pack, *result);
				else
					LOG(Error, "dht not found node");
			});
		}

		Network::nat_pmp _nat_pmp;
		asio::io_service _io_service;
		std::unique_ptr<DHT::dht> _dht;
		std::unique_ptr<Network::network_service> _network_service;
		std::list<contact> _contact_list;
		//std::unordered_map<uint32_t, message> _messages;
		std::unordered_map<uint32_t, friend_request> _friend_requests;
		network_status _network_status;
		
		callbacks _callbacks;
		config _config;
		
	};

}