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

		status send_friend_request(const std::wstring &text, const user_id &user_id)
		{
			return status();
		}

		status add_friend(const user_id &user_id)
		{
			return status();
		}

		status delete_friend(const user_id &user_id)
		{
			return status();
		}

		//TODO: implement
		status send_message(const std::wstring &text, const user_id &user_id)
		{
			message mes;
			//TODO: Init message

			auto result = _network_service->send_message(mes, user_id);
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
		std::unique_ptr<contact_list> _contact_list;

		callbacks _callbacks;
		config _config;
		
	};

}