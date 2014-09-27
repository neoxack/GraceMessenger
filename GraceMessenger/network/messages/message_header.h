#pragma once
#include <stdint.h>

namespace GraceMessenger
{
	namespace Network
	{
		typedef enum type
		{
			
		} m_type;

		typedef struct message_header
		{
			uint16_t type;
			uint16_t size;
		} message_header;
	}
}