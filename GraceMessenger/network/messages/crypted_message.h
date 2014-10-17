#pragma once
#include <stdint.h>
#include "message_header.h"
#include "../constants.h"

namespace GraceMessenger
{
	namespace Network
	{
		
        #pragma pack(1)
		class crypted_message
		{

		public:
			message_header header;
			uint64_t msg_key;
			char enc_data[MAX_MESSAGE_SIZE];
		};
	}
}