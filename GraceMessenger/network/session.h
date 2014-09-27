#pragma once
#include <memory>
#include <array>
#include <asio.hpp>

#include "network_service.h"
#include "message_parser.h"
#include "message_handler.h"
#include "../logger/log.h"
#include "../message.h"



namespace GraceMessenger
{
	class messenger;
	namespace Network
	{	
		class session : public std::enable_shared_from_this<session>
		{

		public:
			session(const session& other) = delete; // non construction-copyable
			session& operator=(const session&) = delete; // non copyable

			explicit session(asio::ip::tcp::socket socket, network_service& network_service, message_handler& handler, user &user) :
				_socket(std::move(socket)),
				_network_service(network_service),
				_message_handler(handler),
				_user(user)
			{
				
			}


			/// Start the first asynchronous operation for the connection.
			void start()
			{
				do_read();
			}

			/// Stop all asynchronous operations associated with the connection.
			void stop()
			{
				_socket.close();
			}



		private:

			void do_read()
			{
				auto self(shared_from_this());
				_socket.async_read_some(asio::buffer(_read_buffer),
					[this, self](asio::error_code ec, std::size_t bytes_transferred)
				{
					if (!ec)
					{
						message_parser::result_type result;
						message_header header;
						result = _message_parser.parse(header, _read_buffer.data(), bytes_transferred);

						if (result == message_parser::good)
						{
							_message_handler.handle(header, _read_buffer.data(), bytes_transferred);
						}
						else if (result == message_parser::bad)
						{
							LOG(Error, "bad message format");
						}
						
						do_read();
					}
					else if (ec != asio::error::operation_aborted)
					{
						_network_service.stop(shared_from_this());
					}
				});
			}

			
			network_service& _network_service;
			message_handler& _message_handler;
			user &_user;
			message_parser _message_parser;
			asio::ip::tcp::socket _socket;

			enum { max_msg = 2048 };
			std::array<uint8_t, max_msg> _read_buffer;
			std::array<uint8_t, max_msg> _write_buffer;
		};
	}
}