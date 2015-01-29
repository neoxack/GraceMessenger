#pragma once
#include "node_id.h"
#include "asio.hpp"


namespace GraceDHT
{
    #pragma pack(1)
	typedef struct 
	{
		node_id id;
		asio::ip::udp::endpoint endpoint;
	} node;
}

