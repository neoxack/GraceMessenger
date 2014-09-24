#pragma once
#include <array>
#include <asio.hpp>
#include <functional>
#include "constants.h"
#include "../logger/log.h"

namespace GraceDHT
{
	typedef std::function<void(const asio::ip::udp::endpoint&, const std::array<char, BUF_SIZE>&, size_t)> handler;

	class network_service
	{
	public:
		network_service(asio::io_service& io_service, const asio::ip::udp::endpoint &endpoint, handler h) :
			_io_service(io_service),
			_socket(io_service, endpoint),
			_handler(h)
		{
			_socket.set_option(asio::ip::udp::socket::reuse_address(true));
		}

		void start()
		{
			_is_started = true;
			recv();
			std::thread([&]() 
			{
				_io_service.run();
			}).detach();		
		}

		void stop()
		{
			_is_started = false;
			_socket.close();
		}

		template<typename Message>
		void send(const Message &message, const asio::ip::udp::endpoint &endpoint)
		{
			message.serialize(_send_buf);
			_to_endpoint = endpoint;
			_socket.async_send_to(asio::buffer(_send_buf, message.size()), _to_endpoint, std::bind(&network_service::handle_send, this,
				std::placeholders::_1,
				std::placeholders::_2));
		}

		void async_run(std::function<void(void)> func)
		{
			_io_service.post(func);
		}
			
	private:

		void handle_send(const asio::error_code& error, std::size_t bytes_sent)
		{
			if (error)
				LOG(Error, error.message());
		}

		void handle_receive(const asio::error_code& error, std::size_t bytes_recvd)
		{
			
			if (bytes_recvd == 0)
			{
				LOG(Info, "bytes_recvd == 0");
			}
			else
			{
				if (!error)
				{
					_handler(_from_endpoint, _recv_buf, bytes_recvd);
				}
				else
					LOG(Error, error.message());
			}
		
			if (_is_started) 
			{
				recv();
			}
		}

		void recv()
		{
			_socket.async_receive_from(asio::buffer(_recv_buf, BUF_SIZE), _from_endpoint, std::bind(&network_service::handle_receive, this,
				std::placeholders::_1,
				std::placeholders::_2));
		}

		asio::io_service& _io_service;
		bool _is_started;
		handler _handler;
		asio::ip::udp::socket _socket;
		asio::ip::udp::endpoint _to_endpoint;
		asio::ip::udp::endpoint _from_endpoint;
		std::array<char, BUF_SIZE> _send_buf;
		std::array<char, BUF_SIZE> _recv_buf;
	};
}
