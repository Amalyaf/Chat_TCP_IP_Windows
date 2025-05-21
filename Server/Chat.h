#pragma once
#include <vector>
#include "Server.h"
#include "Logger.h"

constexpr auto userData = "C:/Windows/Temp/Chat/userData.txt";	// файл для хранения данных пользователей 
constexpr auto user_count = "C:/Windows/Temp/Chat/user_count.txt";	// файл для хранения кол-ва зарегистрированных пользователей
constexpr auto private_message = "C:/Windows/Temp/Chat/private_message.txt";	// файл для хранения непросмотренных личных сообщений
constexpr auto public_message = "C:/Windows/Temp/Chat/public_message.txt";	// файл для хранения непрочитанных общих сообщений
constexpr auto prvt_msg_count = "C:/Windows/Temp/Chat/private_message_count.txt";   // файл для хранения кол-ва личных непрочитанных сообщений
constexpr auto pblc_msg_count = "C:/Windows/Temp/Chat/public_message_count.txt";   // файл для хранения кол-ва общих непрочитанных сообщений
extern std::string status_connect;

class Chat
{
private:
	std::vector<Users> allUsers; // все зарегистрированные пользователи
	std::string _recipient; //получатель сообщения
	std::string _sender; // отправитель сообщения
	std::string _login; // логин авторизованного пользователя
	std::string _password; // пароль авторизованного пользователя
	std::vector<Message>allMessage; // все новые приватные сообщения
	std::vector<Message>allPublicMessage; // все новые общие сообщения
	std::vector<Message>viewedMessage; // просмотренные сообщения
	bool _status = false; // если был выполнен вход статус = true, в противном случае - false.
	Server server;
	Logger log;
	std::shared_mutex mutex;
	int count_users = 0; // кол-во зарегистрированных пользователей
public:
	Chat(); // конструктор
	~Chat(); // деструктор
	void writeUsers() const; // метод для записи данных о пользователях в файл
	void writeMessage() const; // метод для записи данных о сообщениях в файл
	void readUsers(); // метод для чтения данных о пользователях из файла
	void readUsers_DB(); // метод для чтения данных о пользователях из БД
	void readPrivateMessage(); // метод для чтения личных сообщений из файла
	void readPublicMessage(); // метод для чтения общих сообщений из файла		
	void readPrivateMessage_DB(std::string login); // метод для чтения личных сообщений из БД
	void readPublicMessage_DB(std::string login); // метод для чтения общих сообщений из БД		
	int getReadUsersStatus(); // метод выводит 1, если есть файл userData, иначе -1
	int getReadUsersStatus_DB(); // метод выводит 1, если есть строка в таблице Users в БД, иначе -1
	void getChat(); // метод выводит данные пользователя
	void enter(); // авторизация пользователя
	void registration(); //  регистрация пользователя
	void sendPrivateMessage(); // отправка личных сообщений
	void sendPublicMessage(); // отправка публичных сообщений
	bool getstatus(); // метод выводит статус авторизации (_status)
	void exit(); // разлогинить авторизованного пользователя
	void printMessage(std::string recipient); // вывод на экран новых сообщений
	void printMessage_DB(std::string recipient); // вывод на экран новых сообщений из БД
	void deletePrivateMessage(std::string recipient); // перемещение прочитанных личных сообщений в viewedMessage и их удаление из allMessage
	void deletePublicMessage(std::string recipient); // удаление прочитанных общих сообщений из allPublicMessage
	void printAllMessage(); // вывод на экран всех просмотренных личных сообщений
	void start(); // начало работы программы
};