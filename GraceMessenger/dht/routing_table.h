#pragma once
#include <array>
#include <mutex>

#include "node.h"
#include "../boolinq.h"
#include "constants.h"
#include "node_entry.h"


namespace GraceDHT
{
	class routing_table
	{
	public:

		routing_table(const node *main_node) :_main_node(main_node)
		{
		}

		void update(const node& node)
		{
			m.lock();
			if (node.id != _main_node->id)
			{
				auto findIter = std::find_if(_nodes.begin(), _nodes.end(), [&](const node_entry &n){return n.node.id == node.id; });
				if (findIter == _nodes.end())
				{
					if (_nodes.size() < K)
					{
						_nodes.push_back(node_entry(node));
					}
					else
					{
						//TODO: bucket full, need ping first
						_nodes.pop_front();
						_nodes.push_front(node_entry(node));
					}
				}
				else
				{
					findIter->node.endpoint = node.endpoint;
					_nodes.splice(_nodes.end(), _nodes, findIter);
				}
			}
			m.unlock();
		}

		const node* find_node(const node_id &id) const
		{	
			auto findIter = std::find_if(_nodes.begin(), _nodes.end(), [&](const node_entry &n){return n.node.id == id; });
			if (findIter == _nodes.end())
			{
				return nullptr;
			}
			return &(findIter->node);
		}

		std::vector<const node*> find_closest_nodes(const node_id &id, size_t max_results) const
		{
			using namespace boolinq;
			if (_nodes.size() != 0) 
			{
				std::vector<const node*> tmpList;
				for (auto &entry : _nodes)
				{
					tmpList.push_back(&entry.node);
				}

				auto dst = from(tmpList)
					.orderBy([&](const node *a){return bit_position(_main_node->id, a->id); })
					.take(max_results)
					.toVector();
				
				return dst;
			}
			return std::vector<const node*>();
		}

		size_t get_nodes_count()
		{
			return _nodes.size();
		}

	private:
		const node *_main_node;
		std::mutex m;
		std::list<node_entry> _nodes;
		
	};
}