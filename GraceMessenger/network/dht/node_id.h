#pragma once

#include <array>
#include <cstdint>
#include "constants.h"

namespace GraceMessenger
{
	namespace DHT
	{
		typedef std::array<uint8_t, USER_ID_LENGTH> node_id;

		inline std::string id_to_string(const node_id &id)
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

		inline node_id id_from_string(const std::string &id)
		{
			node_id result;
			for (size_t i = 0; i < result.size(); i++)
			{
				std::string str(id.c_str() + i * 2, (id.c_str() + i * 2) + 2);
				result[i] = static_cast<uint8_t>(strtol(str.c_str(), nullptr, 16));
			}

			return result;
		}
	}
}

