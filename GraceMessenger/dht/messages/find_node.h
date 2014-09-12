#pragma once
#include "message_header.h"
#include "../constants.h"
#include "../node.h"

namespace GraceDHT
{
	namespace Messages
	{
		using namespace asio::ip;

		template <bool IsRequest>
		class find_node
		{
			
		public:

			find_node(const node &finder, const node_id &find_id, uint32_t transaction_id, uint32_t ttl) :
				finder(finder),
				find_id(find_id), 
				ttl(ttl),
				header(transaction_id, sizeof(ttl) + sizeof(message_header) + sizeof(node_id) + sizeof(node))
			{
				header.type = IsRequest ? FIND_NODE_REQUEST : FIND_NODE_RESPONSE;
			}

			find_node(const node &finder, const node_id &find_id, uint32_t transaction_id, const node &node) :
				finder(finder),
				find_id(find_id), 
				found_node(node),
				header(transaction_id, sizeof(ttl) + sizeof(message_header) + sizeof(node_id) + sizeof(node) * 2)
			{
				header.type = IsRequest ? FIND_NODE_REQUEST : FIND_NODE_RESPONSE;
			}

			find_node()
			{
			}

			void serialize(std::array<char, BUF_SIZE> &buf) const
			{
				std::memcpy(buf.data(), this, size());
			}

			bool parse(const std::array<char, BUF_SIZE> &buf, const message_header &head)
			{
				std::memcpy(this, buf.data(), head.size);
				return true;
			}

			size_t size() const
			{
				return sizeof(node_id) + sizeof(message_header) + sizeof(node) + sizeof(node) + sizeof(ttl);
			}

			message_header header;
			node finder;
			node_id find_id;
			uint16_t ttl;
			node found_node;
			
		};

	}

}