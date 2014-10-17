#pragma once
#include <stdint.h>

namespace GraceMessenger
{
	namespace Network
	{
		typedef enum type
		{
			AddFriend = 0x6FF9,
			Crypted = 0xBD95
		} m_type;


		#pragma pack(1)
		typedef struct message_header
		{
			uint16_t type;
			uint16_t size;
		} message_header;
	}
}