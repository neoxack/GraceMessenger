#pragma once
#include <stdint.h>
#include <codecvt>

#include "message_header.h"
#include "../../user_id.h"
#include "../constants.h"

namespace GraceMessenger
{
	namespace Network
	{
		#pragma pack(1)
		class add_friend_message
		{
		public:
			add_friend_message(const user_id &sender, const std::wstring &mes):
				sender(sender)
			{
				header.type = AddFriend;
				header.size = sizeof(message_header) + sizeof(user_id) + mes.length() * 2 + 2;
				static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::string encoded = converter.to_bytes(mes);
				memcpy(message, encoded.c_str(), encoded.length());
			}

			add_friend_message(){}

			void serialize(std::array<uint8_t, MAX_MESSAGE_SIZE> &buf) const
			{
				std::memcpy(buf.data(), this, header.size);
			}

			bool parse(const std::array<uint8_t, MAX_MESSAGE_SIZE> &buf, const message_header &h)
			{
				std::memcpy(this, buf.data(), h.size);
				return true;
			}

			std::wstring get_message() const
			{
				static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(message);
			}

			message_header header;
			user_id sender;
			char message[MAX_MESSAGE_SIZE];
		};
	}
}