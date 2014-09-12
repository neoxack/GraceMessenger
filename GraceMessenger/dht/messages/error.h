#pragma once

#include <stdint.h>
#include <cstring>
#include "message_header.h"

namespace GraceDHT
{
	namespace Messages
	{
		enum error_code
		{
			ERROR_203 = 0x203,
			ERROR_204 = 0x204
		};

#pragma pack(1)
		struct error
		{
			uint32_t code;
		};

		typedef struct error error;
	}
}