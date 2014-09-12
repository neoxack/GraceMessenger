#pragma once
#include "node_id.h"
#include "asio.hpp"


namespace GraceDHT
{
    #pragma pack(1)
	struct node
	{
		node_id id;
		asio::ip::udp::endpoint endpoint;
	};

	typedef struct node node;
}

