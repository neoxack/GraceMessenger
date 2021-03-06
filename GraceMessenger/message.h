#pragma once

#include <string>
#include <stdint.h>

#include "user.h"
#include "utils.h"


namespace GraceMessenger
{
	struct message
	{
	public:
		message(const user_id &user, const std::wstring &text):
			contact(user),
			content(text),
			input(false)
		{
			id = Crypto::get_random();
			timestamp = get_timestamp();
		}

		message(){};

		bool operator==(const message& n) const
		{ 
			return (id == n.id); 
		}

		uint32_t id;
		time_t timestamp;
		user_id contact;
		std::wstring content;
		bool input;
	};

}