#pragma once

#include <string>
#include <stdint.h>

#include "user.h"

namespace GraceMessenger
{

	class message
	{
	public:

	private:
		uint32_t _id;
		uint64_t _timestamp;
		user *_author;
		std::string _text;
	};

}