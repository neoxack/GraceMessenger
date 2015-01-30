#pragma once

#include <stdint.h>
#include <asio/ip/udp.hpp>
#include "../node.h"


namespace GraceMessenger
{
	namespace DHT
	{
		typedef enum type
		{
			PING_REQUEST = 0xB937,
			PING_RESPONSE = 0x2965,
			GET_NODES = 0xA543,
			SEND_NODES = 0x0E79
		} m_type;

		const uint8_t USER_ID_LENGTH = 32;

		#pragma pack(1)
		typedef struct 
		{
			uint16_t type;
			uint16_t size;
			uint8_t sender[USER_ID_LENGTH];
		} dht_header;

		#pragma pack(1)
		class ping_packet
		{
		public:
			dht_header header;

			ping_packet(const uint8_t *sender)
			{
				memcpy(header.sender, sender, USER_ID_LENGTH);
				header.type = PING_REQUEST;
				header.size = sizeof(dht_header);
			}

		};

		#pragma pack(1)
		class pong_packet
		{
		public:
			dht_header header;

			pong_packet(const uint8_t *sender)
			{
				memcpy(header.sender, sender, USER_ID_LENGTH);
				header.type = PING_RESPONSE;
				header.size = sizeof(dht_header);
			}
		};

		#pragma pack(1)
		class get_nodes_packet
		{
		public:
			dht_header header;
			uint8_t requested_id[USER_ID_LENGTH];

			get_nodes_packet(const uint8_t *sender, const uint8_t *request_id)
			{
				memcpy(header.sender, sender, USER_ID_LENGTH);
				memcpy(requested_id, request_id, USER_ID_LENGTH);
				header.type = GET_NODES;
				header.size = sizeof(dht_header) + USER_ID_LENGTH;
			}

		};

		#pragma pack(1)
		typedef struct
		{
			uint8_t id[USER_ID_LENGTH];
			asio::ip::udp::endpoint endpoint;
		} dht_node;

		const uint8_t MAX_NODES_IN_RESPONSE = 8;

        #pragma pack(1)
		class send_nodes_packet
		{
		public:
			dht_header header;
			uint16_t nodes_count;
			dht_node nodes[MAX_NODES_IN_RESPONSE];

			send_nodes_packet(const uint8_t *sender, const std::vector<const node*> &finded_nodes)
			{
				memcpy(header.sender, sender, USER_ID_LENGTH);
				header.type = SEND_NODES;
				nodes_count = finded_nodes.size();
				for (size_t i = 0; i < nodes_count; i++)
				{
					memcpy(nodes[i].id, finded_nodes[i]->id.data(), USER_ID_LENGTH);
					nodes[i].endpoint = finded_nodes[i]->endpoint;
				}
				header.size = sizeof(dht_header) + sizeof(nodes_count) + sizeof(dht_node) * nodes_count;
			}

		};


	}
}