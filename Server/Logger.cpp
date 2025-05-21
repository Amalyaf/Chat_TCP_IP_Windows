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
	time_t time = std::time(nullptr); // ���������� ��� �������� �������� �������
	std::stringstream ss; // ��� �������������� ������� � ������

	ss << std::put_time(std::localtime(&time), "%F %T: ");

	fileLog.clear();  // ����� ���������
	fileLog.seekp(0, std::ios::end);  // ������ � ����� �����

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
		// ���������� ������ �����
		while (std::getline(fileLog, str)) {
			std::cout << str << std::endl;
		}
	}
	mutex.unlock_shared();
}

