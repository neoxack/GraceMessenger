#pragma once
#include "json11.hpp"

namespace GraceMessenger
{
	typedef struct
	{
		std::string id;
		std::string address;
		uint16_t port;

		json11::Json to_json() const
		{
			return json11::Json::object
			{ 
				{ "id", id },
				{ "address", address },
				{ "port", port }
			};
		}

	} config_bootstrap_node;

	typedef struct
	{
		std::string id;
		std::string address;
		std::string port;
	} congig_contact;

	typedef struct
	{
		uint16_t dht_port;
		user user;
		std::vector<config_bootstrap_node> bootstrap_nodes;

	} config;

}