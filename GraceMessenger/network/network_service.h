#pragma once

#include "../status.h"
#include "../message.h"
#include "session.h"
#include "../user.h"
#include <unordered_set>

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

			/// Add the specified session to the manager and start it.
			void start(std::shared_ptr<session> s)
			{
				
			}

			/// Stop the specified session.
			void stop(std::shared_ptr<session> s)
			{
				
			}

			/// Stop all sessions.
			void stop_all()
			{
				
			}


		private:
			std::unordered_set<std::shared_ptr<session>> sessions_;
		};
	}

}