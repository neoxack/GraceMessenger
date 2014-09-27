#pragma once

#include "../status.h"
#include "../message.h"
#include "session.h"
#include "../user.h"

namespace GraceMessenger
{
	class message;
	namespace Network
	{
		class session;
		class network_service
		{
		public:
			status send_message(const GraceMessenger::message &mesage, const user &user)
			{
				return status();
			}

			void stop(std::shared_ptr<session> session)
			{

			}

		private:

		};
	}

}