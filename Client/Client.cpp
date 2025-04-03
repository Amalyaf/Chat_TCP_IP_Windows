#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
}

int Client::init()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost", PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server!\n";
        WSACleanup();
        return 1;
    }
}

void Client::Write()
{
    message.clear();
    std::getline(std::cin, message);  // Для ввода строки с пробелами

    if (message == "end") {
        send(ConnectSocket, message.c_str(), message.size(), 0);
        std::cout << "Client Exit." << std::endl;
        close_socket();
    }
    size_t bytes = send(ConnectSocket, message.c_str(), message.size(), 0);
}

void Client::Read()
{
    message.clear();
    // Буфер для получения данных от сервера
    char buffer[MESSAGE_LENGTH];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_received = recv(ConnectSocket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Завершаем строку нулевым символом
        message = buffer;

        if (message == "\nВход не выполнен!\n" || message == "Exit") {
            status = "Exit";
            //close_socket();
            return;
        }
        std::cout << message;
    }
    else {
        std::cout << "Failed to receive data from the server." << std::endl;
    }
}

void Client::close_socket()
{
    std::cout << "Close socket!" << std::endl;
    closesocket(ConnectSocket);
    WSACleanup();
}

void Client::start()
{
    init();
    while (true) {
        Read();
        if (status == "Exit") {
            break;
        }
        Write();
    }
    close_socket();
}