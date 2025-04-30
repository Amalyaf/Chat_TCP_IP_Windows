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


class Logger
{
private:
	const char* logger = "C:/Windows/Temp/Chat/log.txt";	// ���� ��� �������� �����
	std::fstream fileLog = std::fstream(logger, std::ios::in | std::ios::out | std::ios::app);
	time_t time = std::time(nullptr); // ���������� ��� �������� �������� �������
	std::stringstream ss; // ��� �������������� ������� � ������
	std::shared_mutex mutex;
public:
	Logger();
	~Logger();
	void WriteLog(std::string str);
	void ReadLog();
};

