#pragma once
#include "Logger.h"

const std::string LogFileName = "log.log";

#define LOG(Level, Message) log::log_msg(Level, Message, __FUNCTION__)


class log
{
public:
	static void log_msg(Level nLevel, const std::string &message, const std::string &func)
	{
		logger_instance()->log(nLevel, message, func);
	}

	static void log_msg(const std::string &message)
	{
		log_msg(Info, message, "");
	}

private:
	static logger* logger_instance()
	{
		static logger oLog(LogFileName);
		return &oLog;
	}
};