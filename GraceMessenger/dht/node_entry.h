#pragma once

#include "node.h"

namespace GraceDHT
{

	class node_entry
	{
	public:
		node_entry(const node &n) :node(n)
		{
			
		}

		node node;
		uint64_t last_activity;
	};

}

