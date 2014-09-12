#pragma once
#include "message_header.h"
#include "../constants.h"

namespace GraceDHT
{
	namespace Messages
	{
		template <bool IsRequest>
		class ping
		{
		public:

			ping(uint32_t transaction_id, uint32_t random) :
				random(random), header(transaction_id, size())
			{
				header.type = IsRequest ? PING_REQUEST : PING_RESPONSE;
			}

			ping(){};

			void serialize(std::array<char, BUF_SIZE> &buf) const
			{
				std::memcpy(buf.data(), this, sizeof(ping));
			}

			bool parse(const std::array<char, BUF_SIZE> &buf, const message_header &head)
			{
				header = head;
				std::memcpy(&random, buf.data() + sizeof(message_header), sizeof(uint32_t));
				return true;
			}

			size_t size() const
			{
				return sizeof(random) + sizeof(message_header);
			}

		
			message_header header;
			uint32_t random;
		};

	}
	
}

