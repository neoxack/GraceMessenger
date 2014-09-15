#pragma once
#include <memory>
#include <array>
#include <asio.hpp>
#include "../message.h"


namespace GraceMessenger
{
	class session : public std::enable_shared_from_this<session>
	{

	public:
		session(const session& other) = delete; // non construction-copyable
		session& operator=(const session&) = delete; // non copyable



	private:
		asio::ip::tcp::socket _socket;
		enum { max_msg = 1024 };
		std::array<uint8_t, max_msg> _read_buffer;
		std::array<uint8_t, max_msg> _write_buffer;
		Crypto::shared_key _shared_key;
	};

}