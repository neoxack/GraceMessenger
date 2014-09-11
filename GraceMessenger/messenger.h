#pragma once

#include "status.h"
#include "message.h"
#include "callbacks.h"
#include "contact_list.h"

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
		callbacks _callbacks;
		contact_list _contact_list;
	};

}