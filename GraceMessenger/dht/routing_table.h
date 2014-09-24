#pragma once
#include <array>
#include <mutex>

#include "node.h"
#include "../boolinq.h"
#include "constants.h"
#include "node_entry.h"
#include "network_service.h"
#include  "messages/ping.h"


namespace GraceDHT
{
	class routing_table
	{
	public:

		routing_table(const node &main_node, network_service &network_service) :_main_node(main_node), _network_service(network_service)
		{
		}

		void update(const node& node)
		{
			std::lock_guard<std::mutex> lock(_m);
			if (node.id != _main_node.id)
			{
				auto findIter = std::find_if(_nodes.begin(), _nodes.end(), [&](const node_entry &n){return n.node.id == node.id; });
				if (findIter == _nodes.end())
				{
					node_entry entry(node);
					entry.last_activity = get_timestamp();

					if (_nodes.size() < K)
					{
						_nodes.push_back(entry);
					}
					else
					{
						auto n = _nodes.begin();
						ping(n->node);
						_nodes.pop_front();
						_nodes.push_front(entry);
					}
				}
				else
				{
					findIter->node.endpoint = node.endpoint;
					_nodes.splice(_nodes.end(), _nodes, findIter);
				}
			}
		}

		const node* find_node(const node_id &id)
		{	
			std::lock_guard<std::mutex> lock(_m);
			auto findIter = std::find_if(_nodes.begin(), _nodes.end(), [&](const node_entry &n){return n.node.id == id; });
			if (findIter == _nodes.end())
			{
				return nullptr;
			}
			return &(findIter->node);
		}

		std::vector<const node*> find_closest_nodes(const node_id &id, size_t max_results)
		{
			std::lock_guard<std::mutex> lock(_m);
			using namespace boolinq;
			if (_nodes.size() != 0) 
			{
				std::vector<const node*> tmpList;
				for (auto &entry : _nodes)
				{
					tmpList.push_back(&entry.node);
				}

				auto dst = from(tmpList)
					.orderBy([&](const node *a){return bit_position(id, a->id); })
					.take(max_results)
					.toVector();
				
				return dst;
			}
			return std::vector<const node*>();
		}

		const node* get_random_good_node()
		{
			std::lock_guard<std::mutex> lock(_m);
			using namespace boolinq;
			if (_nodes.size() != 0)
			{
				std::vector<const node_entry*> tmpList;
				for (auto &entry : _nodes)
				{
					tmpList.push_back(&entry);
				}

				auto current_time = get_timestamp();
				auto dst = from(tmpList)
					.where([&](const node_entry *a)
					{
						auto diff = current_time - a->last_activity;
						return diff < EXPIRED_TIME / 2;
					})
					.toVector();
				if (dst.size() == 0)
					return nullptr;

				auto random = get_random();
				auto index = random % dst.size();
				return &(dst[index]->node);
			}
			return nullptr;
		}

		size_t get_nodes_count() const
		{
			return _nodes.size();
		}

		void ping(const node &node)
		{
			Messages::ping<true> ping_message(get_random(), get_random());
			ping_message.header.sender_id = _main_node.id;
			_network_service.send(ping_message, node.endpoint);
		}

		void ping_nodes()
		{	
			std::lock_guard<std::mutex> lock(_m);
			auto current_time = get_timestamp();
			_nodes.remove_if([&](const node_entry& entry)
			{
				auto diff = current_time - entry.last_activity;
				if (diff > EXPIRED_TIME) return true;
				return false;
			});
			for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
			{
				auto diff = current_time - it->last_activity;
				if (diff > PING_PERIOD)
				{
					ping(it->node);
				}		
			}
		}

	private:
		std::mutex _m;
		const node &_main_node;
	    network_service &_network_service;
		std::list<node_entry> _nodes;
		
	};
}