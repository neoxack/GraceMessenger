#pragma once
#include <stdint.h>
#include "messages/message_header.h"

namespace GraceMessenger
{
	namespace Network
	{
		class message_parser
		{
		public:
			enum result_type { good, bad, indeterminate };

			message_parser()
			{

			}

			result_type parse(message_header &result, uint8_t *buffer, size_t length)
			{
				return good;
			}

			/// Result of parse.

		};
	}
}