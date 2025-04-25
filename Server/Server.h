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

#define MESSAGE_LENGTH 4096 // ������������ ������ ������ ��� ������
#define PORT "7777" // ����� ������������ ���� ����� �����

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
    SQLHANDLE sqlConnHandle{ nullptr }; // ���������� ��� ���������� � ����� ������
    SQLHANDLE sqlEnvHandle{ nullptr }; // ���������� ��������� ���� ������
    SQLHANDLE sqlStmtHandle{ nullptr };  // ���������� ��� ���������� �������� � ���� ������
    SQLHANDLE sqlStmtHandle1{ nullptr };
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // ������ ��� ���� �������� �� ������� API ODBC
public:
    Server();
    ~Server();
    int init(); // ������������� �������
    int init_socket(); // ������������� ������
    int init_DB(); // ������������� ��
    void Write(std::string text); // �������� ������ �������
    std::string Read(); // ������ ������ �� �������
    void exit(); // �������� ������
    void INSERT_Users(Users us); // ������ ������ � ������� Users/ Users_pswd
    void INSERT_prvt_message(Message msg); // ������ ������ � ������� private_message
    void INSERT_pblc_message(Message msg); // ������ ������ � ������� public_message
    int Select_Users_DB_status(const std::wstring& request); // ������� ��� ������ get_Users_DB_status()
    void Delete_prvt_msg_DB(int id); // �������� ��� ����������� ��������� �� ������� private_message
    void Delete_pblc_msg_DB(int id); // �������� ��� ����������� ��������� �� ������� public_message
    int get_ID_DB(std::string login); // ��������� id �� ������
    void get_Users_DB(); // ����� ������� ������ �� ������� Users
    void get_Users_pswd_DB(); // ����� ������� ������ �� ������� Users_pswd
    void get_private_message_DB(std::string login); // ����� ������� ������ �� ������� private_message
    void get_public_message_DB(std::string login); // ����� ������� ������ �� ������� public_message
    int get_Users_DB_status(); // ���� ���� ������ � ������� Users, �� ���������� 1, ����� -1
    void close_DB(); // �������� ��
};