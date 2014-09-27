#pragma once

#include <memory>

#include "status.h"
#include "message.h"
#include "callbacks.h"
#include "contact_list.h"
#include "dht/dht.h"
#include "logger/log.h"
#include "network/network_service.h"
#include "config.h"

namespace GraceMessenger
{
	class messenger
	{
	public:

		messenger(const config &config, const callbacks &callbacks) :
			_config(config),
			_callbacks(callbacks)
		{
			
		}
		~messenger(){}

		//TODO: implement
		status send_message(const message &mes, const user &user)
		{
			auto result =  _network_service->send_message(mes, user);
			if (result.is_ok())
			{
				_callbacks.message_sent(&mes);
			}
			else
			{
				_callbacks.message_send_error(&mes, &result);
			}
		}

		
		


	private:
		std::unique_ptr<GraceDHT::dht> _dht;
		std::unique_ptr<Network::network_service> _network_service;
		callbacks _callbacks;
		config _config;
		contact_list _contact_list;
	};

}