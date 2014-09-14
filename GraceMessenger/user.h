#pragma once

#include "user_id.h"
#include <asio/ip/tcp.hpp>
#include "crypto/crypto.h"

namespace GraceMessenger
{

	class user
	{
	public:

	private:
		user_id _id;
		asio::ip::tcp::endpoint _endpoint;
		Crypto::shared_key _shared_key;
	};

}