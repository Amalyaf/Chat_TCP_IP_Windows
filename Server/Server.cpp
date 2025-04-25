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
    // Выделяем дескриптор для базы данных
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        close_DB();

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        close_DB();

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        close_DB();

    std::cout << "Попытка подключения к SQL Server...\n";

    // Устанавливаем соединение с сервером  
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
        std::cout << "Успешное подключение к SQL Server\n";
        break;

    case SQL_INVALID_HANDLE:
    case SQL_ERROR:
        std::cout << "Не удалось подключиться к SQL Server\n";
        close_DB();

    default:
        break;
    }

    // Если соединение не установлено, то выходим из программы
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
        close_DB();
        return 1;
    }
        
    std::cout << "\nВыполнение запроса T-SQL...\n";

    // Если выполнение запроса с ошибками, то выходим из программы
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"SELECT * from users", SQL_NTS)) {
        std::cout << "Ошибка запроса SQL Server \n";
        close_DB();
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
    msg.clear();
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

void Server::DataBase_Connect()
{
    // Выделяем дескриптор для базы данных
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        close_DB();

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        close_DB();

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        close_DB();

    std::cout << "Attempting connection to SQL Server...\n";

    // Устанавливаем соединение с сервером  
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

    // Если соединение не установлено, то выходим из программы
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        close_DB();

    std::cout << "\nExecuting T-SQL query...\n";

    // Если выполнение запроса с ошибками, то выходим из программы
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
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::string l = us.getLogin();
    std::string p = us.getPassword();
    std::string n = us.getName();  

    std::wstring name(n.begin(), n.end());
    std::wstring login(l.begin(), l.end());
    std::wstring password(p.begin(), p.end());
    
    std::wstring wusers_qwery = L"INSERT INTO users ( name, login) VALUES ('" + name + L"', '" + login + L"')";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wusers_qwery.c_str(), SQL_NTS)) {
        std::cout << "Success insert Users! \n";
    }
    else {
        std::cout << "Error insert Users!\n";
    }

    // Получаем последний сгенерированный ID
    if (SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"SELECT LAST_INSERT_ID()", SQL_NTS) != SQL_SUCCESS)
        std::cout << "Error last ID!\n";

    SQLINTEGER id = -1;
    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &id, 0, nullptr);
    if (SQLFetch(sqlStmtHandle) != SQL_SUCCESS) {
        std::cout << "Error last ID!\n";
    }

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::wstring wusers_pswd_qwery = L"INSERT INTO users_pswd (user_id, password) VALUES ('" + std::to_wstring(id) + L"','" + password + L"')";
    
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wusers_pswd_qwery.c_str(), SQL_NTS)) {
        std::cout << "Success insert Users_pswd! \n";
    }
    else {
        std::cout << "Error insert Users_pswd!\n";
    }

    get_ID_DB(l);
}


void Server::INSERT_prvt_message(Message msg)
{
    std::string send = msg.getSender();
    std::string rec = msg.getRecipient();
    std::string txt = msg.getText();

    int sender_id = get_ID_DB(send);
    int recipient_id = get_ID_DB(rec);

    /*std::wstring sender(send.begin(), send.end());
    std::wstring recipient(rec.begin(), rec.end());*/
    std::wstring message(txt.begin(), txt.end());

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::wstring wprvt_message = L"INSERT INTO private_message ( user_id_sender, user_id_recipient, message) VALUES ('" + std::to_wstring(sender_id) + L"', '" + std::to_wstring(recipient_id) + L"', '" + message + L"')";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wprvt_message.c_str(), SQL_NTS)) {
        std::cout << "Success insert private_message! \n";
    }
    else {
        std::cout << "Error insert private_message!\n";
    }
}

void Server::INSERT_pblc_message(Message msg)
{
    std::string send = msg.getSender();
    std::string rec = msg.getRecipient();
    std::string txt = msg.getText();

    int sender_id = get_ID_DB(send);
    int recipient_id = get_ID_DB(rec);


    /*std::wstring sender(send.begin(), send.end());
    std::wstring recipient(rec.begin(), rec.end());*/
    std::wstring message(txt.begin(), txt.end());

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::wstring wpublc_message = L"INSERT INTO public_message ( user_id_sender, user_id_recipient, message) VALUES ('" + std::to_wstring(sender_id) + L"', '" + std::to_wstring(recipient_id) + L"', '" + message + L"')";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wpublc_message.c_str(), SQL_NTS)) {
        std::cout << "Success insert public_message! \n";
    }
    else {
        std::cout << "Error insert public_message!\n";
    }
}

void Server::Select_Users_DB(const std::wstring& request)
{
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select! \n";
    }
    else {
        std::cout << "Error select!\n";
        return;
    }

    //Объявление структуры данных
    SQLLEN sql_str_length;
    // Переменная для хранения числа столбцов
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLVARCHAR     V_OD_name[240];
    SQLVARCHAR    V_OD_login[240];

    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_C_CHAR, &V_OD_login, SQL_RESULT_LEN, &sql_str_length);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_C_CHAR, &V_OD_name, SQL_RESULT_LEN, &sql_str_length);

    // Получим значение числа столбцов
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);

    while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        //Выведем на экран данные          
        std::cout << "id: " << V_OD_id << ", login: " << V_OD_login << ", name: " << V_OD_name << std::endl;
    }

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
}


void Server::Select_Users_pswd_DB(const std::wstring& request)
{
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select! \n";
    }
    else {
        std::cout << "Error select!\n";
        return;
    }

    //Объявление структуры данных
    SQLLEN sql_str_length;
    // Переменная для хранения числа столбцов
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLINTEGER     V_OD_user_id[240];
    SQLVARCHAR    V_OD_password[240];



    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_C_CHAR, &V_OD_user_id, SQL_RESULT_LEN, &sql_str_length);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_C_CHAR, &V_OD_password, SQL_RESULT_LEN, &sql_str_length);

    // Получим значение числа столбцов
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);

    while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        //Выведем на экран данные          
        std::cout << "id: " << V_OD_id << ", login: " << V_OD_user_id << ", name: " << V_OD_password << std::endl;
    }

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
}


int Server::Select_Users_DB_status(const std::wstring& request)
{
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select_Users_status! \n";
    }
    else {
        std::cout << "Error select_Users_status!\n";
        return -1;
    }

    //Объявление структуры данных
    SQLLEN sql_str_length;
    // Переменная для хранения числа столбцов
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLVARCHAR     V_OD_name[240];
    SQLVARCHAR    V_OD_login[240];

    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_C_CHAR, &V_OD_login, SQL_RESULT_LEN, &sql_str_length);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_C_CHAR, &V_OD_name, SQL_RESULT_LEN, &sql_str_length);

    // Получим значение числа столбцов
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);

    if (sql_str_length != 0) {
        return 1;
    }

    else {
        return -1;
    }
}

void Server::Delete_prvt_msg_DB(int id)
{
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::wstring wquery = L"Delete from private_message where user_id_recipient = '" + std::to_wstring(id) + L"'";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wquery.c_str(), SQL_NTS)) {
        std::cout << "Success delete from private_message! \n";
    }
    else {
        std::cout << "Error delete from private_message!\n";
        return ;
    }
}

void Server::Delete_pblc_msg_DB(int id)
{
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT

    std::wstring wquery = L"Delete from public_message where user_id_recipient = '" + std::to_wstring(id) + L"'";

    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wquery.c_str(), SQL_NTS)) {
        std::cout << "Success delete from public_message! \n";
    }
    else {
        std::cout << "Error delete from public_message!\n";
        return;
    }
}

int Server::get_ID_DB(std::string l)
{
    //// Получаем id
    //std::wstring wusers_pswd_qwery = L"SELECT id from users where login = '" + std::to_wstring(login) + L"'";
    //if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wusers_pswd_qwery.c_str(), SQL_NTS)) {
    //    std::cout << "Success insert Users! \n";
    //}
    //else {
    //    std::cout << "Error insert Users!\n";
    //}

    /*std::string lgn = us._login;
    std::wstring login(lgn.begin(), lgn.end());*/
    std::wstring login(l.begin(), l.end());
    std::wstring query = L"SELECT id FROM users WHERE login = '" + login + L"'";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    if (SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        std::cout << "Ошибка при выполнении SELECT по логину!\n";
        return 1;
    }

    SQLINTEGER id;
    SQLLEN len;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &id, 0, nullptr);


    if (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        std::cout << "User found:\n";
        std::cout << "ID: " << id << std::endl;
        return id;
    }
    else {
        std::cout << "Пользователь с логином '" << l << "' не найден.\n";
    }
}

void Server::get_Users_DB()
{
    Select_Users_DB(L"Select * from users");
}

void Server::get_Users_pswd_DB()
{
    Select_Users_pswd_DB(L"Select * from users_pswd");
}

void Server::get_private_message_DB(std::string l)
{
    std::cout << "\n\nВход в функцию get_private_message_DB\n\n";
    int recipient_id = get_ID_DB(l);
    std::cout << "ID recipient private_message " << recipient_id << std::endl;
    std::wstring request = L"SELECT * from private_message";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select! \n";
    }
    else {
        std::cout << "Error select!\n";
        return;
    }

    //Объявление структуры данных
    SQLLEN sql_str_length;
    // Переменная для хранения числа столбцов
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLINTEGER     V_OD_sender_id;
    SQLINTEGER    V_OD_recipient_id;
    SQLVARCHAR    V_OD_message[240];

    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_INTEGER, &V_OD_sender_id, sizeof(V_OD_sender_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_INTEGER, &V_OD_recipient_id, sizeof(V_OD_recipient_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 4, SQL_C_CHAR, &V_OD_message, SQL_RESULT_LEN, &sql_str_length);


    // Получим значение числа столбцов
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);

    int count = 0;
    while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        //Выведем на экран данные 
        if (recipient_id == V_OD_recipient_id) {
            count++;
            if (count == 1) {
                Write("\nУ Вас есть новые личные сообщения!");
            }
            std::cout << "id: " << V_OD_id << ", user_sender: " << V_OD_sender_id << ", message: " << V_OD_message << std::endl;
            Write("\nОтправитель: ");
            Write(std::to_string(V_OD_sender_id));
            Write("\nСообщение: ");
            std::string str = reinterpret_cast<char*>(V_OD_message);
            Write(str);
            Write("\n");
        }
    }
    Delete_prvt_msg_DB(recipient_id);
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
}

void Server::get_public_message_DB(std::string l)
{
    int recipient_id = get_ID_DB(l);
    std::cout << "ID recipient public_message " << recipient_id << std::endl;
    std::wstring request = L"SELECT * from public_message";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
    if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)request.c_str(), SQL_NTS)) {
        std::cout << "Success Select! \n";
    }
    else {
        std::cout << "Error select!\n";
        return;
    }

    //Объявление структуры данных
    SQLLEN sql_str_length;
    // Переменная для хранения числа столбцов
    SQLSMALLINT    V_OD_colanz, V_OD_rowcount;
    SQLINTEGER   V_OD_err, V_OD_id;
    SQLINTEGER     V_OD_sender_id;
    SQLINTEGER    V_OD_recipient_id;
    SQLVARCHAR    V_OD_message[240];

    V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_INTEGER, &V_OD_sender_id, sizeof(V_OD_sender_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 3, SQL_INTEGER, &V_OD_recipient_id, sizeof(V_OD_recipient_id), nullptr);
    V_OD_err = SQLBindCol(sqlStmtHandle, 4, SQL_C_CHAR, &V_OD_message, SQL_RESULT_LEN, &sql_str_length);

    // Получим значение числа столбцов
    V_OD_err = SQLNumResultCols(sqlStmtHandle, &V_OD_colanz);
    V_OD_err = SQLRowCount(sqlStmtHandle, &sql_str_length);

    int count = 0;
    while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
        //Выведем на экран данные          
        if (recipient_id == V_OD_recipient_id) {
            count++;
            if (count == 1) {
                Write("\nУ Вас есть новые личные сообщения!");
            }
            std::cout << "id: " << V_OD_id << ", user_sender: " << V_OD_sender_id << ", message: " << V_OD_message << std::endl;
            Write("\nОтправитель: ");
            Write(std::to_string(V_OD_sender_id));
            Write("\nСообщение: ");
            std::string str = reinterpret_cast<char*>(V_OD_message);
            Write(str);
            Write("\n");
        }
    }
    Delete_pblc_msg_DB(recipient_id);
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE); // очищаем перед новым INSERT
}

int Server::get_Users_DB_status()
{
    if (Select_Users_DB_status(L"SELECT EXISTS(SELECT 1 FROM Users WHERE id = 1)") == 1) {
        return 1;
    }
    else {
        return -1;
    }
}

void Server::close_DB()
{
    // Закрываем соединение и выходим из программы
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    // Пауза перед закрытием консольного приложения
    std::cout << "\nPress any key to exit...";
}

