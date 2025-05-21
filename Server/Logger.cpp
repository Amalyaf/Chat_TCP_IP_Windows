#include "Logger.h"

Logger::Logger()
{
	fileLog.open(logger, std::ios::in | std::ios::out | std::ios::app);
}

Logger::~Logger()
{
	fileLog.close();
}

void Logger::WriteLog(std::string str)
{
	mutex.lock();
	time_t time = std::time(nullptr); // переменная для хранения текущего времени
	std::stringstream ss; // для преобразования времени в строку

	ss << std::put_time(std::localtime(&time), "%F %T: ");

	fileLog.clear();  // Сброс состояния
	fileLog.seekp(0, std::ios::end);  // Писать в конец файла

	auto s = ss.str();
	fileLog << s << str << std::endl;
	mutex.unlock();
}

void Logger::ReadLog()
{
	mutex.lock_shared();
	fileLog.clear();
	fileLog.seekg(0);
	if (fileLog.is_open()) {
		std::string str;
		// Построчное чтение файла
		while (std::getline(fileLog, str)) {
			std::cout << str << std::endl;
		}
	}
	mutex.unlock_shared();
}

