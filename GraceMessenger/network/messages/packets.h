#pragma once

#include <stdint.h>

#include "../../user_id.h"
#include "../../crypto/aes256.h"
#include "../../crypto/crypto.h"

namespace GraceMessenger
{
	namespace Network
	{
		const size_t MAX_CRYPTED_DATA_LENGTH = 32 * 1024;

		#pragma pack(1)
		typedef struct
		{
			uint16_t size;
			uint8_t sender[USER_ID_LENGTH];
		} header;

		#pragma pack(1)
		typedef struct
		{
			uint16_t size;
			uint16_t type;
			uint32_t id;
		} data_packet_header;

		enum data_packet_types
		{
			ChangeStatus = 1,
			FriendRequest = 2,
			AddFriend = 3,
			DeleteFriend = 4,
			SimpleTextMessage = 5,
			MessageDelivered = 6,
			Ping = 7,
			Pong = 8
		};

		#pragma pack(1)
		class crypted_packet
		{
		public:
			header header;
			uint8_t crypted_data[MAX_CRYPTED_DATA_LENGTH];

			void serialize(uint8_t *buf) const
			{
				memcpy(buf, this, header.size);
			}

			void parse(const uint8_t* buffer, size_t size)
			{
				memcpy(this, buffer, size);
			}

			crypted_packet(uint8_t *sender)
			{
				memcpy(header.sender, sender, USER_ID_LENGTH);
			}

			crypted_packet(){}

			data_packet_header *decrypt(const Crypto::shared_key &shared_key)
			{
				size_t crypted_data_size = header.size - sizeof(header);
				size_t rounds = crypted_data_size / 16;
				size_t o = crypted_data_size % 16;
				if (o > 0) rounds++;

				uint8_t aes_key[Crypto::KEY_LENGTH];
				memcpy(aes_key, shared_key.data(), Crypto::KEY_LENGTH);

				aes256_context ctx;
				aes256_init(&ctx, aes_key);
				for (size_t i = 0; i < rounds; i++)
				{
					aes256_decrypt_ecb(&ctx, crypted_data + i * 16);
				}
				aes256_done(&ctx);

				data_packet_header *data_packet_header_ptr = reinterpret_cast<data_packet_header *>(crypted_data);
				return data_packet_header_ptr;

			}

			template <typename T>
			void crypt(const T &pack, const Crypto::shared_key &shared_key)
			{
				size_t rounds = pack.header.size / 16;
				size_t o = pack.header.size % 16;
				if (o > 0) rounds++;

				uint8_t aes_key[Crypto::KEY_LENGTH];
				memcpy(aes_key, shared_key.data(), Crypto::KEY_LENGTH);

				aes256_context ctx;
				aes256_init(&ctx, aes_key);
				memcpy(crypted_data, &pack, pack.header.size);

				for (size_t i = 0; i < rounds; i++)
				{
					aes256_encrypt_ecb(&ctx, crypted_data + i * 16);
				}
				aes256_done(&ctx);
				header.size = sizeof(header) + rounds * 16;
			}

		};


		//-------------DATA PACKETS----------------

		#pragma pack(1)
		class change_status_packet
		{
		public:
			data_packet_header header;
			uint16_t status;

			change_status_packet(const uint16_t st)
			{
				header.type = ChangeStatus;
				status = st;
				size_t data_packet_header_size = sizeof(data_packet_header);
				header.size = data_packet_header_size + sizeof(status);
			}

		};

		#pragma pack(1)
		class friend_request_packet
		{
		public:
			data_packet_header header;
			uint16_t text_length;
			wchar_t text[16000];
			
			friend_request_packet(uint32_t id, const std::wstring &msg_text)
			{
				header.type = FriendRequest;		
				text_length = msg_text.size();
				header.id = id;
				size_t data_packet_header_size = sizeof(data_packet_header);
				header.size = data_packet_header_size + sizeof(text_length) + text_length * sizeof(wchar_t);
				memcpy(text, msg_text.c_str(), text_length * sizeof(wchar_t));
			}

		};

		#pragma pack(1)
		class add_friend_packet
		{
		public:
			data_packet_header header;

			add_friend_packet(uint32_t id)
			{
				header.type = AddFriend;
				header.id = id;
				size_t data_packet_header_size = sizeof(data_packet_header);
				header.size = data_packet_header_size;
			}

		};

		#pragma pack(1)
		class delete_friend_packet
		{
		public:
			data_packet_header header;

			delete_friend_packet(uint32_t id)
			{
				header.type = DeleteFriend;
				header.id = id;
				size_t data_packet_header_size = sizeof(data_packet_header);
				header.size = data_packet_header_size;
			}

		};


		#pragma pack(1)
		class simple_text_message_packet
		{
		public:
			data_packet_header header;
			uint16_t text_length;
			wchar_t text[16000];

			simple_text_message_packet(uint32_t id, const std::wstring &msg_text)
			{
				header.type = SimpleTextMessage;
				header.id = id;
				size_t data_packet_header_size = sizeof(data_packet_header);
				text_length = msg_text.size();
				header.size = data_packet_header_size + sizeof(text_length) + text_length * sizeof(wchar_t);
				memcpy(text, msg_text.c_str(), text_length * sizeof(wchar_t));
			}

		};


        #pragma pack(1)
		class message_delivered_packet
		{
		public:
			data_packet_header header;
			uint16_t msg_type;

			message_delivered_packet(uint32_t id, uint16_t msg_t)
			{
				header.type = MessageDelivered;
				header.id = id;
				msg_type = msg_t;
				header.size = sizeof(data_packet_header) + sizeof(msg_type);
			}

		};


		#pragma pack(1)
		class ping_packet
		{
		public:
			data_packet_header header;

			ping_packet()
			{
				header.type = Ping;
				header.size = sizeof(data_packet_header);
			}

		};

		#pragma pack(1)
		class pong_packet
		{
		public:
			data_packet_header header;

			pong_packet()
			{
				header.type = Pong;
				header.size = sizeof(data_packet_header);
			}

		};

	}
}