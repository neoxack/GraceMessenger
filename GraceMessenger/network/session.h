#pragma once
#include <memory>
#include <array>
#include <asio.hpp>

#include "network_service.h"
#include "message_parser.h"
#include "message_handler.h"
#include "../logger/log.h"
#include "../message.h"
#include "messages/add_friend_message.h"
#include "../contact_list.h"
#include "../utils.h"
#include "crypto_service.h"


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

			explicit session(messenger &messenger, asio::ip::tcp::socket socket, network_service& network_service, message_handler& handler, user &user, contact_list &contact_list) :
				_messenger(messenger),
				_socket(std::move(socket)),
				_network_service(network_service),
				_user(user),
				_contact_list(contact_list)
			{
				_session_id = get_random64();
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

			void handle_add_friend_message(const add_friend_message &message)
			{
				if (_contact_list.contains(message.sender))
				{
					
				}
				else
				{
					//TODO: friend request callback
				}
			}

			void handle(const message_header &header, const std::array<uint8_t, MAX_MESSAGE_SIZE> &data)
			{
				switch (header.type)
				{
					case AddFriend:
					{
						add_friend_message message;
						message.parse(data, header);
						handle_add_friend_message(message);
						break;
					}
					case Crypted:
					{
						crypted_message crypted_message;
						crypted_message.parse(data, header);
						std::array<uint8_t, MAX_MESSAGE_SIZE> decrypted_array;
						crypto_service::decrypt(crypted_message, _user._shared_key, decrypted_array);

						message_parser::result_type result;
						message_header decrypted_header;
						result = _message_parser.parse(decrypted_header, decrypted_array, header.size);

						if (result == message_parser::good)
						{
							handle(decrypted_header, decrypted_array);
						}
						else if (result == message_parser::bad)
						{
							LOG(Error, "bad crypted message format");
						}

						break;
					}
				}
			}

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
						result = _message_parser.parse(header, _read_buffer, bytes_transferred);

						if (result == message_parser::good)
						{
							handle(header, _read_buffer);
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

			uint64_t _session_id;
			messenger &_messenger;
			network_service &_network_service;
			user &_user;
			contact_list &_contact_list;
			message_parser _message_parser;
			asio::ip::tcp::socket _socket;

			enum { max_msg = 2048 };
			std::array<uint8_t, MAX_MESSAGE_SIZE> _read_buffer;
			std::array<uint8_t, MAX_MESSAGE_SIZE> _write_buffer;
		};
	}
}