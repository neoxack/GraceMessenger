#pragma once
#include <cstdint>
#include <random>


namespace GraceMessenger
{
	inline uint64_t get_random64()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<uint64_t> dis;
		return dis(gen);
	}

	inline uint32_t get_random32()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<uint32_t> dis;
		return dis(gen);
	}

}