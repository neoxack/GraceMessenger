#pragma once

#include <memory>

#include "status.h"
#include "message.h"
#include "callbacks.h"
#include "contact_list.h"
#include "dht/dht.h"

namespace GraceMessenger
{
	class messenger
	{
	public:

		messenger(){}
		~messenger(){}

		//TODO: implement
		status send_message(const message &mesage)
		{
			return status();
		}



	private:
		std::unique_ptr<GraceDHT::dht> _dht;
		callbacks _callbacks;
		contact_list _contact_list;
	};

}