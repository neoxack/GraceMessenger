#pragma once
#include <array>
#include <asio.hpp>
#include <functional>
#include "constants.h"

namespace GraceDHT
{
	typedef std::function<void(const asio::ip::udp::endpoint&, const std::array<char, BUF_SIZE>&, size_t)> handler;

	class network_service
	{
	public:
		network_service(asio::io_service& io_service, const asio::ip::udp::endpoint &endpoint, handler h) :
			_socket(io_service, endpoint),
			_handler(h),
			_my_endpoint(endpoint)
		{
			_socket.set_option(asio::ip::udp::socket::reuse_address(true));
		}

		void start()
		{
			_is_started = true;
			std::thread([&]() 
			{
				while (_is_started)
				{
					asio::error_code error;
					size_t bytes_recvd = _socket.receive_from(asio::buffer(_recv_buf, BUF_SIZE), _from_endpoint, 0, error);
					if (!error)
						_handler(_from_endpoint, _recv_buf, bytes_recvd);
				}
			}).detach();		
		}

		void stop()
		{
			_is_started = false;
		}

		template<typename Message>
		void send(const Message &message, const asio::ip::udp::endpoint &endpoint)
		{
			message.serialize(_send_buf);
			_to_endpoint = endpoint;
			asio::error_code error;
			size_t bytes_sent = _socket.send_to(asio::buffer(_send_buf, message.size()), _to_endpoint, 0, error);
		}
			
	private:
		bool _is_started;
		handler _handler;
		asio::ip::udp::socket _socket;
		asio::ip::udp::endpoint _to_endpoint;
		asio::ip::udp::endpoint _from_endpoint;
		asio::ip::udp::endpoint _my_endpoint;
		std::array<char, BUF_SIZE> _send_buf;
		std::array<char, BUF_SIZE> _recv_buf;
	};
}