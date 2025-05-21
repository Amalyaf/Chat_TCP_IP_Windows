#pragma once
#define _CRT_SECURE_NO_WARNINGS //��� ���������� �������������� ����������� �� ������������� ������������ ������� � ���������� CRT (C Run-Time Library)
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono> // ��� ��������� �������� �������
#include <format> // ��� ������ ������� � ����������� �������
#include <sstream>
#include <iomanip>
#include <shared_mutex>
#include <thread>

class Logger
{
private:
	const char* logger = "C:/Windows/Temp/Chat/log.txt";	// ���� ��� �������� �����
	std::fstream fileLog;
	std::shared_mutex mutex;
public:
	Logger();
	~Logger();
	void WriteLog(std::string str);
	void ReadLog();
};

