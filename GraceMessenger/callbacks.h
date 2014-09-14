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
		std::function<void(const message *mes)> message_received;
		std::function<void(const message *mes, const status* status)> message_send_error;
	} callbacks;

}