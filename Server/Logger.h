#pragma once
#define _CRT_SECURE_NO_WARNINGS //для отключения предупреждений компилятора об использовании небезопасных функций в библиотеке CRT (C Run-Time Library)
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono> // Для получения текущего времени
#include <format> // Для вывода времени в определённом формате
#include <sstream>
#include <iomanip>
#include <shared_mutex>
#include <thread>

class Logger
{
private:
	const char* logger = "C:/Windows/Temp/Chat/log.txt";	// файл для хранения логов
	std::fstream fileLog;
	std::shared_mutex mutex;
public:
	Logger();
	~Logger();
	void WriteLog(std::string str);
	void ReadLog();
};

