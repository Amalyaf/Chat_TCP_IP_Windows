#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

int Server::init()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

void Server::Write(std::string msg)
{
    //msg.clear();
       // Ввод сообщения от сервера
       //std::cout << "Enter the message you want to send to the client: " << std::endl;
       //std::getline(std::cin, msg);  // Используем getline для ввода строки с пробелами

       // Отправка данных клиенту
    size_t bytes_sent = send(ClientSocket, msg.c_str(), msg.size(), 0);
    if (bytes_sent < 0) {
        std::cout << "Failed to send data to the client!" << std::endl;
    }
    //msg.clear();
}

std::string Server::Read()
{
    msg.clear();
    // Чтение данных от клиента
    char buffer[MESSAGE_LENGTH];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_received = recv(ClientSocket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Завершаем строку нулевым символом
        msg = buffer;
        std::cout << msg << std::endl;
    }
    else {
        return "Error reading from client!";
    }
    return msg;
}

void Server::exit()
{
    closesocket(ClientSocket);
}