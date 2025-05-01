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
	ss.str("");
	ss << std::put_time(std::localtime(&time), "%F %T: ");
	auto s = ss.str();
	fileLog << s << str << std::endl;
	mutex.unlock();
}

void Logger::ReadLog()
{
	if (fileLog.is_open()) {
		std::string str;
		// Построчное чтение файла
		mutex.lock_shared();
		while (std::getline(fileLog, str)) {
			std::cout << str << std::endl;
		}
		mutex.unlock_shared();
	}
}
