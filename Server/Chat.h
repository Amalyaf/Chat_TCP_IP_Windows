#pragma once
#include <vector>
#include "Server.h"
#include "Logger.h"

constexpr auto userData = "C:/Windows/Temp/Chat/userData.txt";	// ���� ��� �������� ������ ������������� 
constexpr auto user_count = "C:/Windows/Temp/Chat/user_count.txt";	// ���� ��� �������� ���-�� ������������������ �������������
constexpr auto private_message = "C:/Windows/Temp/Chat/private_message.txt";	// ���� ��� �������� ��������������� ������ ���������
constexpr auto public_message = "C:/Windows/Temp/Chat/public_message.txt";	// ���� ��� �������� ������������� ����� ���������
constexpr auto prvt_msg_count = "C:/Windows/Temp/Chat/private_message_count.txt";   // ���� ��� �������� ���-�� ������ ������������� ���������
constexpr auto pblc_msg_count = "C:/Windows/Temp/Chat/public_message_count.txt";   // ���� ��� �������� ���-�� ����� ������������� ���������
extern std::string status_connect;

class Chat
{
private:
	std::vector<Users> allUsers; // ��� ������������������ ������������
	std::string _recipient; //���������� ���������
	std::string _sender; // ����������� ���������
	std::string _login; // ����� ��������������� ������������
	std::string _password; // ������ ��������������� ������������
	std::vector<Message>allMessage; // ��� ����� ��������� ���������
	std::vector<Message>allPublicMessage; // ��� ����� ����� ���������
	std::vector<Message>viewedMessage; // ������������� ���������
	bool _status = false; // ���� ��� �������� ���� ������ = true, � ��������� ������ - false.
	Server server;
	Logger log;
	std::shared_mutex mutex;
	int count_users = 0; // ���-�� ������������������ �������������
public:
	Chat(); // �����������
	~Chat(); // ����������
	void writeUsers() const; // ����� ��� ������ ������ � ������������� � ����
	void writeMessage() const; // ����� ��� ������ ������ � ���������� � ����
	void readUsers(); // ����� ��� ������ ������ � ������������� �� �����
	void readUsers_DB(); // ����� ��� ������ ������ � ������������� �� ��
	void readPrivateMessage(); // ����� ��� ������ ������ ��������� �� �����
	void readPublicMessage(); // ����� ��� ������ ����� ��������� �� �����		
	void readPrivateMessage_DB(std::string login); // ����� ��� ������ ������ ��������� �� ��
	void readPublicMessage_DB(std::string login); // ����� ��� ������ ����� ��������� �� ��		
	int getReadUsersStatus(); // ����� ������� 1, ���� ���� ���� userData, ����� -1
	int getReadUsersStatus_DB(); // ����� ������� 1, ���� ���� ������ � ������� Users � ��, ����� -1
	void getChat(); // ����� ������� ������ ������������
	void enter(); // ����������� ������������
	void registration(); //  ����������� ������������
	void sendPrivateMessage(); // �������� ������ ���������
	void sendPublicMessage(); // �������� ��������� ���������
	bool getstatus(); // ����� ������� ������ ����������� (_status)
	void exit(); // ����������� ��������������� ������������
	void printMessage(std::string recipient); // ����� �� ����� ����� ���������
	void printMessage_DB(std::string recipient); // ����� �� ����� ����� ��������� �� ��
	void deletePrivateMessage(std::string recipient); // ����������� ����������� ������ ��������� � viewedMessage � �� �������� �� allMessage
	void deletePublicMessage(std::string recipient); // �������� ����������� ����� ��������� �� allPublicMessage
	void printAllMessage(); // ����� �� ����� ���� ������������� ������ ���������
	void start(); // ������ ������ ���������
};