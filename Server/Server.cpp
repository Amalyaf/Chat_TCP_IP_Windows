#include "Server.h"

Server::Server()
{
}

Server::~Server()
{

}

int Server::init()
{
    if (init_socket() == 0 && init_DB() == 0) {
        return 0;
    }
}

int Server::init_socket()
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

int Server::init_DB()
{
    // �������� ���������� ��� ���� ������
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        close_DB();

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        close_DB();

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        close_DB();

    std::cout << "������� ����������� � SQL Server...\n";

    // ������������� ���������� � ��������  
    switch (SQLDriverConnect(sqlConnHandle,
        GetDesktopWindow(),
        (SQLWCHAR*)L"DRIVER={MySQL ODBC 9.2 ANSI Driver};SERVER=localhost;PORT=3306;DATABASE=chat_db;UID=root;PWD=root;",
        SQL_NTS,
        retconstring,
        1024,
        NULL,
        SQL_DRIVER_COMPLETE)) {


    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        std::cout << "�������� ����������� � SQL Server\n";
        break;

    case SQL_INVALID_HANDLE:
    case SQL_ERROR:
        std::cout << "�� ������� ������������ � SQL Server\n";
        close_DB();

    default:
        break;
    }

    // ���� ���������� �� �����������, �� ������� �� ���������
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
        close_DB();
        return 1;
    }
        
    std::cout << "\n���������� ������� T-SQL...\n";

    // ���� ���������� ������� � ��������, �� ������� �� ���������
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"SELECT * from users", SQL_NTS)) {
        std::cout << "������ ������� SQL Server \n";
        close_DB();
        return 1;
    }

    return 0;
}

void Server::Write(std::string msg)
{
    //msg.clear();
       // ���� ��������� �� �������
       //std::cout << "Enter the message you want to send to the client: " << std::endl;
       //std::getline(std::cin, msg);  // ���������� getline ��� ����� ������ � ���������

       // �������� ������ �������
    size_t bytes_sent = send(ClientSocket, msg.c_str(), msg.size(), 0);
    if (bytes_sent < 0) {
        std::cout << "Failed to send data to the client!" << std::endl;
    }
    msg.clear();
}

std::string Server::Read()
{
    msg.clear();
    // ������ ������ �� �������
    char buffer[MESSAGE_LENGTH];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_received = recv(ClientSocket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // ��������� ������ ������� ��������
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

void Server::DataBase_Connect()
{
    // �������� ���������� ��� ���� ������
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        close_DB();

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        close_DB();

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        close_DB();

    std::cout << "Attempting connection to SQL Server...\n";

    // ������������� ���������� � ��������  
    switch (SQLDriverConnect(sqlConnHandle,
        GetDesktopWindow(),
        (SQLWCHAR*)L"DRIVER={MySQL ODBC 9.2 ANSI Driver};SERVER=localhost;PORT=3306;DATABASE=chat_db;UID=root;PWD=root;",
        SQL_NTS,
        retconstring,
        1024,
        NULL,
        SQL_DRIVER_COMPLETE)) {


    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        std::cout << "Successfully connected to SQL Server\n";
        break;

    case SQL_INVALID_HANDLE:
    case SQL_ERROR:
        std::cout << "Could not connect to SQL Server\n";
        close_DB();

    default:
        break;
    }

    // ���� ���������� �� �����������, �� ������� �� ���������
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        close_DB();

    std::cout << "\nExecuting T-SQL query...\n";

    // ���� ���������� ������� � ��������, �� ������� �� ���������
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"SELECT * from users", SQL_NTS)) {
        std::cout << "Error querying SQL Server \n";
        close_DB();
    }
}

void Server::Create_TABLE()
{
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"create table Chat_DB(id int AUTO_INCREMENT PRIMARY KEY, name varchar(255), login varchar(255), password varchar(255))", SQL_NTS)) {
        std::cout << "Error Create! \n"; 
    }
    else {
        std::cout << "Success create!\n";
    }
}

void Server::INSERT_Users(Users us)
{    
    std::string l = us.getLogin();
    std::string p = us.getPassword();
    std::string n = us.getName();  

    std::wstring name(n.begin(), n.end());
    std::wstring login(l.begin(), l.end());
    std::wstring password(p.begin(), p.end());
    
    std::wstring wusers_qwery = L"INSERT INTO users ( name, login) VALUES ('" + name + L"', '" + login + L"')";
    std::wstring wusers_pswd_qwery = L"INSERT INTO users_pswd (password) VALUES ('" + password + L"')";

    
    //������ ������� ���, ����� � ������� � �������� ���� id ������������, �� ����� ������ ��� ���������� � ������ ������� ���������� V_OD_id
    /*SQLINTEGER   V_OD_err, V_OD_id;
    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    std::wstring wusers_pswd_qwery = L"INSERT INTO users_pswd (user_id, password) VALUES ('" + V_OD_id + L"','" + password + L"')";*/

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wusers_qwery.c_str(), SQL_NTS)) {
        std::cout << "Success insert Users! \n";
    }
    else {
        std::cout << "Error insert Users!\n";
    }
    
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wusers_pswd_qwery.c_str(), SQL_NTS)) {
        std::cout << "Success insert Users_pswd! \n";
    }
    else {
        std::cout << "Error insert Users_pswd!\n";
    }

    //Select_DB(L"Select * from users");
}

void Server::INSERT_prvt_message(Message msg)
{
    std::string send = msg.getSender();
    std::string rec = msg.getRecipient();
    std::string txt = msg.getText();

    std::wstring sender(send.begin(), send.end());
    std::wstring recipient(rec.begin(), rec.end());
    std::wstring message(txt.begin(), txt.end());

    std::wstring wprvt_message = L"INSERT INTO private_message ( user_sender, user_recipient, message) VALUES ('" + sender + L"', '" + recipient + L"', '" + message + L"')";
    
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wprvt_message.c_str(), SQL_NTS)) {
        std::cout << "Success insert private_message! \n";
    }
    else {
        std::cout << "Error insert private_message!\n";
    }
}

void Server::INSERT_publc_message(Message msg)
{
    std::string send = msg.getSender();
    std::string rec = msg.getRecipient();
    std::string txt = msg.getText();

    std::wstring sender(send.begin(), send.end());
    std::wstring recipient(rec.begin(), rec.end());
    std::wstring message(txt.begin(), txt.end());

    std::wstring wpublc_message = L"INSERT INTO public_message ( user_sender, user_recipient, message) VALUES ('" + sender + L"', '" + recipient + L"', '" + message + L"')";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wpublc_message.c_str(), SQL_NTS)) {
        std::cout << "Success insert public_message! \n";
    }
    else {
        std::cout << "Error insert public_message!\n";
    }
}

void Server::Select_DB(const std::wstring& request)
{
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select! \n";
    }
    else {
        std::cout << "Error select!\n";
        return;
    }

    //���������� ��������� ������
    SQLLEN sql_str_length;
    // ���������� ��� �������� ����� ��������
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLVARCHAR     V_OD_name;
    SQLVARCHAR    V_OD_login;
    //SQLCHAR*     V_OD_password;

    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_WCHAR, &V_OD_name, SQL_RESULT_LEN, &sql_str_length);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_WCHAR, &V_OD_login, SQL_RESULT_LEN, &sql_str_length);
    //V_OD_err = SQLBindCol(sqlStmtHandle, 4, SQL_WCHAR, &V_OD_password, SQL_RESULT_LEN, &sql_str_length);

    

    // ������� �������� ����� ��������
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    std::cout << "���������� ��������: " << V_OD_colanz << std::endl;

    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);
    std::cout << "���������� �����: " << sql_str_length << std::endl;

    while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        //������� �� ����� ������          
        std::cout << "id: " << V_OD_id << ", name: " << V_OD_name << ", login: " << V_OD_login << std::endl;
    }
}

void Server::close_DB()
{
    // ��������� ���������� � ������� �� ���������
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    // ����� ����� ��������� ����������� ����������
    std::cout << "\nPress any key to exit...";
}

