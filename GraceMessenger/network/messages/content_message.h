#pragma once
#include <stdint.h>
#include <string>
#include <codecvt>
#include <array>

#include "message_header.h"
#include "../constants.h"
#include "../../utils.h"


namespace GraceMessenger
{
	namespace Network
	{
        #pragma pack(1)
		class content_message
		{
		public:
			content_message(const std::wstring &mes)
			{
				header.type = AddFriend;
				header.size = sizeof(message_header) + sizeof(_timestamp) + sizeof(_id) + mes.length() * 2 + 2;
				static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::string encoded = converter.to_bytes(mes);
				memcpy(message, encoded.c_str(), encoded.length());
				_id = get_random32();
				_timestamp = get_timestamp();
			}

			content_message(){}

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
			uint32_t _id;
			uint64_t _timestamp;
			char message[MAX_MESSAGE_SIZE];
		};
	}
}