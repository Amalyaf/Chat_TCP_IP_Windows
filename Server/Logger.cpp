#include "Logger.h"

Logger::Logger()
{
	fileLog.open(logger, std::ios::in | std::ios::out | std::ios::app);
}

Logger::~Logger()
{
	std::cout << "Удаление и закрытие лога" << std::endl;
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
	fileLog.seekg(0);
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
