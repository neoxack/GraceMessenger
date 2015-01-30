#pragma once
#include <cstdint>
#include <random>
#include <asio.hpp>

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

	inline uint64_t get_timestamp()
	{
		using namespace std::chrono;
		milliseconds ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
		return ms.count();
	}

	inline asio::ip::address get_public_ip()
	{
		asio::io_service netService;
		asio::ip::udp::resolver   resolver(netService);
		asio::ip::udp::resolver::query query(asio::ip::udp::v4(), "google.com", "");
		asio::ip::udp::resolver::iterator endpoints = resolver.resolve(query);
		asio::ip::udp::endpoint ep = *endpoints;
		asio::ip::udp::socket socket(netService);
		socket.connect(ep);
		asio::ip::address addr = socket.local_endpoint().address();
		socket.close();
		return addr;
	}

}