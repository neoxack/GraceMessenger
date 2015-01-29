#pragma once

#include <string>
#include <functional>
#include "message.h"

namespace GraceMessenger
{
	struct friend_request;

	typedef struct
	{
		std::function<void(bool success, int error)> dht_bootstrapped;

		std::function<void(const message *mes)> message_sent;
		std::function<void(const message *mes)> message_received;
		std::function<void(const message *mes)> message_delivered;
		std::function<void(const message *mes)> message_readed;
		std::function<void(const message *mes, const status* status)> message_send_error;

		std::function<void(const friend_request *mes)> friend_request_received;
		std::function<void(const friend_request *mes)> friend_request_sended;
		std::function<void(const friend_request *req, const status* status)> friend_request_send_error;

		std::function<void(const user *user)> typing;
	} callbacks;

}