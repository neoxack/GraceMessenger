#pragma once

#include <stdint.h>
#include <random>

#include "curve25519.h"

namespace GraceMessenger
{
	namespace Crypto
	{
		typedef std::array<uint8_t, 32> shared_key;

		inline uint32_t get_random()
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<uint32_t> dis;
			return dis(gen);
		}

		inline void generate_keypair(uint8_t *private_key, uint8_t *public_key)
		{
			//generate private_key
			for (size_t i = 0; i < 8; i++)
			{
				auto r = get_random();
				*(uint32_t*)(private_key + i * sizeof(uint32_t)) = r;
			}

			private_key[0] &= 248;
			private_key[31] &= 127;
			private_key[31] |= 64;

			//generate public key
			static const uint8_t basepoint[32] = { 9 };
			curve25519(public_key, private_key, basepoint);
		}

		inline void generate_sharedkey(uint8_t *shared_key, uint8_t *mysecret, uint8_t *theirpublic)
		{
			curve25519(shared_key, mysecret, theirpublic);
		}
	}
}