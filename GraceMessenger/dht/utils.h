#pragma once
#include <cstdint>
#include <array>
#include <random>
#include <chrono>

#include "node_id.h"

namespace GraceDHT
{
	inline int32_t log2(int32_t i)
	{
		if (i == 0)
			return -1;

		int bit = 31;
		if ((i & 0xFFFFFF00) == 0)
		{
			i <<= 24;
			bit = 7;
		}
		else if ((i & 0xFFFF0000) == 0)
		{
			i <<= 16;
			bit = 15;
		}
		else if ((i & 0xFF000000) == 0)
		{
			i <<= 8;
			bit = 23;
		}

		if ((i & 0xF0000000) == 0)
		{
			i <<= 4;
			bit -= 4;
		}

		while ((i & 0x80000000) == 0)
		{
			i <<= 1;
			bit--;
		}

		return bit;
	}

	template <size_t SIZE>
	inline int32_t bit_position(const std::array<uint8_t, SIZE> &arr1, const std::array<uint8_t, SIZE> &arr2)
	{
		for (size_t i = 0; i < SIZE; i++)
		{
			int32_t xor = arr1[i] ^ arr2[i];
			if (xor != 0)
			{
				return (SIZE - i - 1)*8 + (log2(xor));
			}
			if (i == (SIZE-1)) return -1;
		}
		return 0;
	}

	inline uint32_t get_random()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<uint32_t> dis;
		return dis(gen);
	}

	inline uint64_t get_timestamp()
	{
		using namespace std::chrono;
		milliseconds ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
		return ms.count();
	}

	inline void fill_random_id(node_id& id)
	{
		for (size_t i = 0; i < id.size(); i += sizeof(uint32_t))
		{
			*reinterpret_cast<uint32_t*>(id.data()+i) = get_random();
		}
	}

	inline std::string node_id_to_string(const node_id &id)
	{
		std::stringstream result;

		for (size_t i = 0; i < id.size(); i++)
		{
			result << std::hex << id[i];
			if (i != id.size() - 1) result << "-";
		}
		return result.str();
	}

	inline node_id node_id_from_string(const std::string &id)
	{
		node_id result;
		sscanf_s(id.c_str(), "%x-%x-%x-%x-%x-%x-%x-%x", &result[0], &result[1], &result[2], &result[3], &result[4], &result[5], &result[6], &result[7]);
		return result;
	}

	inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) 
		{
			elems.push_back(item);
		}
		return elems;
	}


	inline std::vector<std::string> split(const std::string &s, char delim) 
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}
}


namespace std
{
	//hasher for arrays
	template<typename T, size_t N>
	struct hash<array<T, N> >
	{
		typedef array<T, N> argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type& a) const
		{
			hash<T> hasher;
			result_type h = 0;
			for (result_type i = 0; i < N; ++i)
			{
				h = h * 31 + hasher(a[i]);
			}
			return h;
		}
	};
}