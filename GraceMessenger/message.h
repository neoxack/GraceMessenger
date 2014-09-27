#pragma once

#include <string>
#include <stdint.h>

#include "user.h"
#include "network/session.h"

namespace GraceMessenger
{

	class message
	{
	public:
		

	private:
		uint32_t _id;
		uint64_t _timestamp;
		user *_author;
		user *_recipient;
		std::string _text;
	};

}