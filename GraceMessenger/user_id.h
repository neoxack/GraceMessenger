#pragma once

#include <array>
#include <stdint.h>

namespace GraceMessenger
{
	const size_t USER_ID_LENGTH = 32;

	typedef std::array<uint8_t, USER_ID_LENGTH> user_id;

}