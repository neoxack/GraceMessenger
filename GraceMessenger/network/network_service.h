#pragma once

#include "../status.h"
#include "../message.h"
#include "../user.h"

namespace GraceMessenger
{
	class network_service
	{
	public:
		status send_message(const message &mesage, const user &user)
		{
			return status();
		}

	private:
		
	};

}