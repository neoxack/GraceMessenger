#pragma once
#include <stdint.h>

#include "messages/message_header.h"
#include "constants.h"

namespace GraceMessenger
{
	namespace Network
	{
		class message_parser
		{
		public:
			enum result_type { good, bad, indeterminate };

		

			static result_type parse(message_header &result, const std::array<uint8_t, MAX_MESSAGE_SIZE> &data, size_t length)
			{
				if (length < sizeof(message_header)) return bad;
				memcpy(&result, data.data(), sizeof(message_header));
				if (length < result.size) return bad;
				return good;
			}

			/// Result of parse.

		};
	}
}