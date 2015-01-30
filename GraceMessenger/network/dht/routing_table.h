#pragma once
#include <array>

#include "node.h"
#include "../cpplinq.hpp"
#include "network_service.h"
#include "utils.h"


namespace GraceMessenger
{
	namespace DHT
	{
		class routing_table
		{
		public:

			routing_table(const routing_table&) = delete;
			routing_table& operator=(const routing_table&) = delete;

			routing_table(const node &main_node, network_service &network_service) :_main_node(main_node), _network_service(network_service)
			{
			}

			void update(const node_id &id, const asio::ip::udp::endpoint &endpoint)
			{
				if (id != _main_node.id)
				{
					auto current_time = get_timestamp();
					auto findIter = find_if(_nodes.begin(), _nodes.end(), [id](const node &n){return n.id == id; });
					if (findIter == _nodes.end())
					{
						node entry;
						entry.id = id;
						entry.endpoint = endpoint;
						entry.is_good = true;
						entry.last_activity = current_time;

						if (_nodes.size() < K)
						{
							_nodes.push_back(entry);
						}
						else
						{
							auto n = _nodes.begin();
							ping(*n);
							_nodes.pop_front();
							_nodes.push_front(entry);
						}
					}
					else
					{
						findIter->endpoint = endpoint;
						findIter->last_activity = current_time;
						findIter->is_good = true;
						_nodes.splice(_nodes.end(), _nodes, findIter);
					}
				}
			}

			const node* find_node(const node_id &id)
			{
				auto findIter = find_if(_nodes.begin(), _nodes.end(), [id](const node &n){return n.id == id; });
				if (findIter == _nodes.end())
				{
					return nullptr;
				}
				return &(*findIter);
			}

			std::vector<const node*> find_closest_nodes(const node_id &id, size_t max_results)
			{
				using namespace cpplinq;
				if (_nodes.size() != 0)
				{
					std::vector<const node*> tmpList;
					for (auto &entry : _nodes)
					{
						tmpList.push_back(&entry);
					}

					auto dst = from(tmpList)
						>> orderby_ascending([id](const node*a){return bit_position(id, a->id); })
						>> take(max_results)
						>> to_vector();

					return dst;
				}
				return std::vector<const node*>();
			}

			const node* get_random_good_node()
			{
				using namespace cpplinq;
				if (_nodes.size() != 0)
				{
					std::vector<const node*> tmpList;
					for (auto &entry : _nodes)
					{
						tmpList.push_back(&entry);
					}

					auto dst = from(tmpList)
						>> where([](const node *a)
					{
						return a->is_good;
					})
						>> to_vector();
					if (dst.size() == 0)
						return nullptr;

					auto random = get_random();
					auto index = random % dst.size();
					return dst[index];
				}
				return nullptr;
			}

			size_t get_nodes_count() const
			{
				return _nodes.size();
			}

			void ping(const node &node)
			{
				ping_packet ping_pack(_main_node.id.data());
				_network_service.send(&ping_pack.header, node.endpoint);
			}

			void ping_nodes()
			{
				auto current_time = get_timestamp();
				_nodes.remove_if([current_time](const node& entry)
				{
					auto diff = current_time - entry.last_activity;
					if (diff > EXPIRED_TIME) return true;
					return false;
				});
				for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
				{
					auto diff = current_time - it->last_activity;
					if (diff > BAD_PERIOD)
						it->is_good = false;
					if (diff > PING_PERIOD)
					{
						ping(*it);
					}
				}
			}

		private:
			const node &_main_node;
			network_service &_network_service;
			std::list<node> _nodes;

		};
	}
}