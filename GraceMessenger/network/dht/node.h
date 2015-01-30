#pragma once
#include "node_id.h"
#include "asio.hpp"


namespace GraceMessenger
{
	namespace DHT
	{
		typedef struct
		{
			node_id id;
			asio::ip::udp::endpoint endpoint;
			uint64_t last_activity;
			bool is_good;
		} node;
	}
}

