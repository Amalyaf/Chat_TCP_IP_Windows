#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define MESSAGE_LENGTH 4096 // Максимальный размер буфера для данных
#define PORT "7777" // Будем использовать этот номер порта

class Client {
private:
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;
    std::string message;
    std::string status;
public:
    Client();
    ~Client();
    int init();
    void Write();
    void Read();
    void close_socket();
    void start();
};
