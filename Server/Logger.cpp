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
	std::cout << "Метод WRITE()" << std::endl;
	mutex.lock();
	ss.str("");
	ss << std::put_time(std::localtime(&time), "%F %T: ");
	auto s = ss.str();
	fileLog << s << str << std::endl;
	mutex.unlock();
}

void Logger::ReadLog()
{
	std::cout << "Метод READ()" << std::endl;
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

void Logger::start(std::string str)
{
	std::thread t1(&Logger::WriteLog, this, str);
	std::thread t2(&Logger::ReadLog, this);
	t1.join();
	t2.join();
}
