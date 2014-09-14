#pragma once
#include <string>
#include <sstream>
#include <mutex>
#include <memory>
#include <fstream>

// log message levels
enum Level	{ Finest, Finer, Fine, Config, Info, Warning, Severe };

class logger
{
public:
	logger(std::string filename);
	~logger();

	void log(Level nLevel, const std::string &oMessage, const std::string &func);

private:
	std::string getTimestamp();

	std::mutex m_oMutex;
	std::ofstream m_oFile;
};
