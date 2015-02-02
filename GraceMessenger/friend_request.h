#pragma once

#include <stdint.h>

#include "user.h"
#include "utils.h"


namespace GraceMessenger
{
	class friend_request
	{
	public:
		friend_request(const user_id &user, const std::wstring &text, const std::wstring &sender_name) :
			contact(user),
			name(sender_name),
			content(text),
			input(false)
		{
			id = Crypto::get_random();
			timestamp = get_timestamp();
		}

		friend_request(){};

		bool operator==(const friend_request& n) const
		{
			return (id == n.id);
		}

		uint32_t id;
		time_t timestamp;
		user_id contact;
		std::wstring name;
		std::wstring content;
		bool input;
	};

}