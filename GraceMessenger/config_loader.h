#pragma once
#include <string>
#include <fstream>

#include "config.h"
#include "json11.hpp"
#include <codecvt>


namespace GraceMessenger
{

	class config_loader
	{
	public:
		config_loader() = delete;

		static void save_config(const config &config, const std::wstring &path)
		{
			using namespace json11;
			Json obj = Json::object({
				{ "dht_port", config.dht_port },
				{ "user", Json::object({
					{ "id", id_to_string(config.user.id) },
					{ "name", ws2s(config.user.name) },
					{ "private_key", id_to_string(config.user.private_key) }
				}) },
			});
			std::string dump = obj.dump();
			std::ofstream out(path);
			out << dump;
			out.close();
		}

		static config load_config(const std::wstring &path)
		{
			using namespace json11;
			std::string dump;
			std::ifstream in(path);
			std::getline(in, dump);
			config conf;
			std::string err;
			auto json = Json::parse(dump, err);
			conf.dht_port = json["dht_port"].int_value();
			conf.user.id = id_from_string(json["user"]["id"].string_value());
			conf.user.private_key = id_from_string(json["user"]["private_key"].string_value());
			conf.user.name = s2ws(json["user"]["name"].string_value());
			return conf;
		}

	private:
		static std::wstring s2ws(const std::string& str)
		{
			typedef std::codecvt_utf8<wchar_t> convert_typeX;
			static std::wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.from_bytes(str);
		}

		static std::string ws2s(const std::wstring& wstr)
		{
			typedef std::codecvt_utf8<wchar_t> convert_typeX;
			static std::wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.to_bytes(wstr);
		}

		
	};

}