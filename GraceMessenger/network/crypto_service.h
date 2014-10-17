#pragma once
#include <stdint.h>
#include "messages/crypted_message.h"
#include "../user.h"
#include "../crypto/sha1.h"
#include "../crypto/aes256.h"


namespace GraceMessenger
{
	namespace Network
	{
		class crypto_service
		{
		public:
			
			template<typename M>
			crypted_message encrypt(const M &message, const Crypto::shared_key &shared_key)
			{
				crypted_message result;
				result.header.type = Crypted;
				union msg_key key_union = { 0 };

				sha1::calc(&message, message.header.size, key_union.sha1_hash);
				result.msg_key = key_union.key;

				uint8_t aes_key[32];
				generate_aes_key(shared_key, result.msg_key, aes_key);
				
				size_t rounds = message.header.size / 16;
				size_t o = message.header.size % 16;
				if (o > 0) rounds++;

				aes256_context ctx;
				aes256_init(&ctx, aes_key);
				for (size_t i = 0; i < rounds; i++)
				{
					uint8_t block[16];
					memcpy(block, &message + i * 16, 16);
					aes256_encrypt_ecb(&ctx, block);
					memcpy(result.enc_data + i * 16, block, 16);
				}
				aes256_done(&ctx);
				result.header.size = rounds * 16;
				return result;
			}

			template<typename T>
			T decrypt(const crypted_message &crypted, const Crypto::shared_key &shared_key)
			{
				T result;

				return result;
			}

		private:

			void generate_aes_key(const Crypto::shared_key &shared_key, uint64_t msg_key, uint8_t *aes_key)
			{
				uint8_t h1[24];
				memcpy(h1, shared_key.data(), 16);
				memcpy(h1 + 16, &msg_key, sizeof(msg_key));
				uint8_t h1_hash[20];
				sha1::calc(h1, 24, h1_hash);

				uint8_t h2[24];
				memcpy(h2, shared_key.data()+16, 16);
				memcpy(h2 + 16, &msg_key, sizeof(msg_key));
				uint8_t h2_hash[20];
				sha1::calc(h2, 24, h2_hash);
				memcpy(aes_key, h1, 16);
				memcpy(aes_key+16, h2, 16);
			}

			union msg_key
			{
				uint64_t key;
				uint8_t sha1_hash[20];
			};
		};
	}
}