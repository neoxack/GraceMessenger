#pragma once
#include <stdint.h>
#include "messages/message_header.h"

namespace GraceMessenger
{
	namespace Network
	{
		class message_handler
		{
		public:
			message_handler(const message_handler&) = delete;
			message_handler& operator=(const message_handler&) = delete;

			void handle(const message_header& header, uint8_t *buffer, size_t length)
			{
				
			}
		};
	}
}