#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

logger::logger(std::string filename)
{
	m_oFile.open(filename, std::fstream::out | std::fstream::app | std::fstream::ate);
}

logger::~logger()
{
	m_oFile.flush();
	m_oFile.close();
}

std::string logger::getTimestamp()
{
	auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto tm = std::localtime(&in_time_t);
	std::stringstream ss;
	ss << std::put_time(tm, "%Y-%m-%d %X");
	return ss.str();
}

void logger::log(Level nLevel, const std::string &oMessage, const std::string &func)
{
	const static char* LevelStr[] = { "Error", "Config", "Info", "Warning", "Severe" };
	
	m_oMutex.lock();
	m_oFile << '[' << getTimestamp() << ']' 
		<< '[' << LevelStr[nLevel] << ']'
		<< '[' << func << "]\t"
		<< oMessage << std::endl;
	m_oMutex.unlock();
}


