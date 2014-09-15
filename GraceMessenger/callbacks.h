#pragma once

#include <string>
#include <functional>
#include "message.h"

namespace GraceMessenger
{

	typedef struct callbacks
	{
		std::function<void(const message *mes)> message_sent;
		std::function<void(const message *mes)> message_delivered;
		std::function<void(const message *mes)> message_readed;
		std::function<void(const message *mes, const status* status)> message_send_error;

		std::function<void(const message *mes)> friend_request_received;
		std::function<void(const message *mes)> friend_request_sended;

		std::function<void(const user *user)> typing;
	} callbacks;

}