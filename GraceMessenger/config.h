#pragma once

namespace GraceMessenger
{
	struct config
	{
		std::string ip_adress;
		uint16_t dht_port;
		uint16_t messenger_port;
		user user;
	};

}