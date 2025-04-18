#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "Users.h"
#include "Message.h"

// Need to link with Ws2_32.lib 
#pragma comment (lib, "Ws2_32.lib")

#define MESSAGE_LENGTH 4096 // Максимальный размер буфера для данных
#define PORT "7777" // Будем использовать этот номер порта

constexpr auto SQL_RESULT_LEN = 240;
constexpr auto SQL_RETURN_CODE_LEN = 1024;

class Server {
private:
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int iResult;
    std::string msg;
    bool check = true;
    SQLHANDLE sqlConnHandle{ nullptr }; // дескриптор для соединения с базой данных
    SQLHANDLE sqlEnvHandle{ nullptr }; // дескриптор окружения базы данных
    SQLHANDLE sqlStmtHandle{ nullptr };  // дескриптор для выполнения запросов к базе данных
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка для кода возврата из функций API ODBC
public:
    Server();
    ~Server();
    int init();
    void Write(std::string text);
    //void Write(bool check);
    std::string Read();
    void exit();
    void DataBase_Connect();
    void Create_TABLE();
    void INSERT_Users(Users us);
    void INSERT_prvt_message(Message msg);
    void INSERT_publc_message(Message msg);
    void Select_DB(const std::wstring& request);
    void close_DB();
};