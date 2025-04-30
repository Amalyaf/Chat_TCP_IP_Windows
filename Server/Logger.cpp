#include "Logger.h"

Logger::Logger()
{
	fileLog.is_open();
}

Logger::~Logger()
{
	fileLog.close();
}

void Logger::WriteLog(std::string str)
{
	mutex.lock();
	ss << std::put_time(std::localtime(&time), "%F %T: ");
	auto s = ss.str();
	fileLog << s << str << std::endl;
	fileLog.close();
	mutex.unlock();
}
