#pragma once

#include <stdint.h>
#include <cstring>
#include "../utils.h"
#include "../node_id.h"


namespace GraceDHT
{
	namespace Messages
	{
		typedef enum type
		{
			PING_REQUEST = 0xB937,
			PING_RESPONSE = 0x2965,
			FIND_NODE_REQUEST = 0xA543,
			FIND_NODE_RESPONSE = 0x0E79
		} m_type;

		#pragma pack(1)
		struct message_header
		{
			uint16_t type;
			uint32_t transaction_id;
			node_id sender_id;
			uint16_t size;
			uint32_t random;
			uint64_t timestamp;

			message_header(uint32_t transaction_id, uint32_t size) :
				sender_id({}),
				transaction_id(transaction_id), 
				size(size), 
				random(get_random()), 
				timestamp(get_timestamp()), 
				type(0)
			{
			}

			message_header() :sender_id({}), transaction_id(0), size(0), random(0), timestamp(0), type(0)
			{}

			void parse(const char* const data)
			{
				std::memcpy(this, data, sizeof(message_header));
			}
		};

		typedef struct message_header message_header;
	}
}