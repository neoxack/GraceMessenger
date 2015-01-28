#pragma once

#include <array>
#include <stdint.h>
#include <sstream>
#include <iomanip>


namespace GraceMessenger
{
	const size_t USER_ID_LENGTH = 32;

	typedef std::array<uint8_t, USER_ID_LENGTH> user_id;

	inline std::string id_to_string(const user_id &id)
	{
		std::stringstream result;
		result << std::hex << std::setfill('0');
		result.setf(std::ios::uppercase);

		for (size_t i = 0; i < id.size(); i++)
		{
			result << std::setw(2) << static_cast<unsigned int>(id[i]);
		}
		return result.str();
	}

	inline user_id id_from_string(const std::string &id)
	{
		user_id result;
		for (size_t i = 0; i < result.size(); i++)
		{
			std::string str(id.c_str() + i * 2, (id.c_str() + i * 2) + 2);
			result[i] = static_cast<uint8_t>(strtol(str.c_str(), nullptr, 16));
		}

		return result;
	}

}