#pragma once

#include "user_id.h"
#include "crypto/crypto.h"

namespace GraceMessenger
{

	class user
	{
	public:
		user()
		{
			
		}

		user(const std::wstring &u_name):
			name(u_name)
		{
			Crypto::generate_keypair(private_key.data(), id.data());
		}

		user_id id;
		std::wstring name;
		Crypto::private_key private_key;
	};

}