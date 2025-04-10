#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define MESSAGE_LENGTH 4096 // Максимальный размер буфера для данных
#define PORT "7777" // Будем использовать этот номер порта


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
public:
    Server();
    ~Server();
    int init();
    void Write(std::string text);
    //void Write(bool check);
    std::string Read();
    void exit();

};