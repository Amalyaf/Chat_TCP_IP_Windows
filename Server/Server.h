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

#define MESSAGE_LENGTH 4096 // ћаксимальный размер буфера дл€ данных
#define PORT "7777" // Ѕудем использовать этот номер порта

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
    SQLHANDLE sqlConnHandle{ nullptr }; // дескриптор дл€ соединени€ с базой данных
    SQLHANDLE sqlEnvHandle{ nullptr }; // дескриптор окружени€ базы данных
    SQLHANDLE sqlStmtHandle{ nullptr };  // дескриптор дл€ выполнени€ запросов к базе данных
    SQLHANDLE sqlStmtHandle1{ nullptr };
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка дл€ кода возврата из функций API ODBC
public:
    Server();
    ~Server();
    int init(); // инициализаци€ сервера
    int init_socket(); // инициализаци€ сокета
    int init_DB(); // инициализаци€ Ѕƒ
    void Write(std::string text); // отправка данных клиенту
    std::string Read(); // чтение данных от клиента
    void exit(); // закрытие сокета
    void INSERT_Users(Users us); // запись данных в таблицу Users/ Users_pswd
    void INSERT_prvt_message(Message msg); // запись данных в таблицу private_message
    void INSERT_pblc_message(Message msg); // запись данных в таблицу public_message
    int Select_Users_DB_status(const std::wstring& request); // выборка дл€ метода get_Users_DB_status()
    void Delete_prvt_msg_DB(int id); // удаление уже прочитанных сообщений из таблицы private_message
    void Delete_pblc_msg_DB(int id); // удаление уже прочитанных сообщений из таблицы public_message
    int get_ID_DB(std::string login); // получение id по логину
    void get_Users_DB(); // ћетод выводит данные из таблицы Users
    void get_Users_pswd_DB(); // ћетод выводит данные из таблицы Users_pswd
    void get_private_message_DB(std::string login); // ћетод выводит данные из таблицы private_message
    void get_public_message_DB(std::string login); // ћетод выводит данные из таблицы public_message
    int get_Users_DB_status(); // если есть строки в таблице Users, то возвращает 1, иначе -1
    void close_DB(); // закрытие Ѕƒ
};