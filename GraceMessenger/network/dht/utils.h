#pragma once
#include <cstdint>
#include <array>
#include <random>
#include <chrono>

#include "node_id.h"

namespace GraceMessenger
{
	namespace DHT
	{
		
		inline uint64_t id_distance(const std::array<uint8_t, USER_ID_LENGTH> &arr1, const std::array<uint8_t, USER_ID_LENGTH> &arr2)
		{	
			uint64_t *a = (uint64_t *)arr1.data();
			uint64_t *b = (uint64_t *)arr2.data();

			uint64_t x = (*a) ^ (*b);	
			return x;
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
				*reinterpret_cast<uint32_t*>(id.data() + i) = get_random();
			}
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