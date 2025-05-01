#include "Chat.h"
#include <algorithm>
#include "bad_login.h"
#include "bad_password.h"
#include <filesystem>
#include <fstream>
#include <cstdlib>

std::string status_connect = "No";

Chat::Chat()
{
	// ���� ���� ���� � ������� ����� ������������������ �������������, �� ������� ������ ��� ���������� ������ �� ������
	if (getReadUsersStatus() == 1) {
		readUsers();
		readPrivateMessage();
		readPublicMessage();
	}
	if (server.init() == 0) {
		log.WriteLog("Server successfully connected!");
		status_connect = "Yes";
	}
	else {
		log.WriteLog("Server not connected!");
		status_connect = "No";
	}
}

Chat::~Chat() {
	writeUsers(); // ����� ��� ������ ������ ������������������ ������������� � ����
	writeMessage(); // ����� ��� ������ ������ � ��������� ��������� � ��������� �����
	if (status_connect == "Yes") {
		server.Write("Exit");
	}
	server.exit();
}


void Chat::getChat()
{
	for (std::vector<Users>::iterator it = allUsers.begin(); it < allUsers.end(); it++)
	{
		server.Write("\n����� ������������: ");
		server.Write(it->getLogin());
		server.Write("\n������: ");
		server.Write(it->getPassword());
		server.Write("\n���: ");
		server.Write(it->getName());
		server.Write("\n");
	}
}

void Chat::enter()
{
	std::string c = "y";
	while (c != "n")
	{
		try
		{
			server.Write("\n��� ����� ������� �����: ");
			_login = server.Read();
			Users user;
			user._login = _login;

			//// ��� ������ �� ��
			if (server.Select_Users_DB(_login) != 1) {
				std::cout << " BADLOGIN!!!" << std::endl;
				throw BadLogin();
			}

			// ��� ������ �� �����
			/*std::vector<Users>::iterator result = find(allUsers.begin(), allUsers.end(), user);
			if (result == allUsers.end())
			{
				throw BadLogin();
			}*/

			else
			{
				//user = *result;
				server.Write("������� ������: ");
				_password = server.Read();

				// ��� ������ �� ��
				if (server.Select_UsersPswd_DB(_login, _password) != 1) {
					log.WriteLog("����� ������������ ������ ��� ������� ������ ������������ " + _login);
					throw BadPassword();
				}

				// ��� ������ �� �����
				/*if (user._password != _password)
				{
					log.WriteLog("����� ������������ ������ ��� ������� ������ ������������ " + _login);
					throw BadPassword();
				}*/
				else
				{
					_status = true;
					c = "n";
					log.WriteLog("�������� ���� ��� ������� ������� ������������ " + _login);
					printMessage_DB(_login);
				}
			}
		}
		catch (BadLogin& e)
		{
			server.Write(e.what());
			c = server.Read();
		}
		catch (BadPassword& e)
		{
			server.Write(e.what());
			c = server.Read();
		}
	}
}

void Chat::registration()
{
	std::string c = "y";
	while (c != "n")
	{
		Users user;
		server.Write("\n����� ���������� � ���!\n����������� ������ ������������!\n������� �����: ");
		user.setLogin(server.Read());
		server.Write("������� ������: ");
		user.setPassword(server.Read());
		server.Write("������� ���: ");
		user.setName(server.Read());
		std::vector<Users>::iterator result = find(allUsers.begin(), allUsers.end(), user);
		if (result != allUsers.end())
		{
			server.Write("\n������������ � ����� ������� ��� ����������!\n������ ��������� �������?(y/n): ");
			c = server.Read();
		}
		else
		{
			count_users++;
			allUsers.push_back(user);
			server.INSERT_Users(user); // ��������� ������ ������������ � ��
			c = "n";
		}
	}
}

void Chat::sendPrivateMessage()
{
	std::string c = "y";
	_sender = _login;
	while (c != "n")
	{
		server.Write("����: ");
		_recipient = server.Read();
		Users user;
		user._login = _recipient;
		std::vector<Users>::iterator result = find(allUsers.begin(), allUsers.end(), user);
		if (result == allUsers.end())
		{
			server.Write("\n���������� �� ������!\n������ ��������� �������?(y/n): ");
			c = server.Read();
		}

		else
		{
			Message message;
			message._recipient = _recipient;
			message._sender = _sender;
			server.Write("\n������� ���������:\n");
			message.setMessage(server.Read());
			allMessage.push_back(message);
			server.INSERT_prvt_message(message);
			c = "n";
		}
	}
}

void Chat::sendPublicMessage()
{
	Message message;
	message._sender = _login;
	server.Write("\n������� ��������� ���������:\n");
	message.setMessage(server.Read());
	for (std::vector<Users>::iterator it = allUsers.begin(); it < allUsers.end(); it++)
	{
		if (it->_login != _login)
		{
			message._recipient = it->_login;
			allPublicMessage.push_back(message);
			server.INSERT_pblc_message(message);
		}
	}

	//message._recipient = "all";
	viewedMessage.push_back(message);
}

void Chat::printMessage(std::string recipient)
{
	int count = 0;
	for (std::vector<Message>::iterator it = allMessage.begin(); it < allMessage.end(); it++)
	{
		if (it->_recipient == recipient)
		{
			count++;
			if (count == 1)
			{
				server.Write("\n------------------------------------------------------\n� ��� ���� ����� ������ ���������: ");
				//server.Write("� ��� ���� ����� ������ ���������: ");
			}
			server.Write("\n�����������: ");
			server.Write(it->_sender);
			server.Write("\n����������: ");
			server.Write(it->_recipient);
			server.Write("\n���������: ");
			server.Write(it->_message);
			server.Write("\n");
		}
		
	}
	if (count != 0)
	{
		server.Write("\n------------------------------------------------------\n");
		deletePrivateMessage(recipient);
	}
	count = 0;
	for (std::vector<Message>::iterator it = allPublicMessage.begin(); it < allPublicMessage.end(); it++)
	{
		if (it->_recipient == recipient)
		{
			count++;
			if (count == 1)
			{
				server.Write("\n------------------------------------------------------\n� ��� ���� ����� ����� ���������: ");
				//server.Write("� ��� ���� ����� ����� ���������: ");
			}
			server.Write("\n�����������: ");
			server.Write(it->_sender);
			server.Write("\n���������: ");
			server.Write(it->_message);
			server.Write("\n");
		}
		
	}
	if (count != 0)
	{
		server.Write("\n------------------------------------------------------\n");
		deletePublicMessage(recipient);
	}
}

void Chat::printMessage_DB(std::string recipient)
{
	readPrivateMessage_DB(recipient);
	readPublicMessage_DB(recipient);
}

void Chat::deletePrivateMessage(std::string recipient)
{
	for (std::vector<Message>::iterator it = allMessage.begin(); it < allMessage.end();)
	{
		if (it->_recipient == recipient)
		{
			viewedMessage.push_back(*it);
			it = allMessage.erase(it);
		}
		else
			++it;
	}
}

void Chat::deletePublicMessage(std::string recipient)
{
	for (std::vector<Message>::iterator it = allPublicMessage.begin(); it < allPublicMessage.end();)
	{
		if (it->_recipient == recipient)
		{
			it = allPublicMessage.erase(it);
		}
		else
			++it;
	}
}

void Chat::printAllMessage()
{
	for (std::vector<Message>::iterator it = viewedMessage.begin(); it != viewedMessage.end(); it++)
	{
		server.Write("\n�����������: ");
		server.Write(it->_sender);
		server.Write("\n����������: ");
		server.Write(it->_recipient);
		server.Write("\n���������: ");
		server.Write(it->_message);
	}
}

bool Chat::getstatus()
{
	return _status;
}

void Chat::exit()
{
	_status = false;
	_login.clear();
	_password.clear();
}

void Chat::readUsers() {

	std::fstream file = std::fstream(userData, std::ios::in);

	if (!file)
	{
		server.Write("No file");
		return;
	}

	const std::string delimiter = ":";

	if (file.is_open()) {
		std::string line;
		std::string title;
		std::string value;
		int user_cnt = 0;

		std::fstream user_cnt_file(user_count, std::ios::in);
		if (user_cnt_file.is_open()) {
			std::string line;
			getline(user_cnt_file, line);
			user_cnt = stoi(line);
		}
		for (int i = 0; i < user_cnt; i++) {
			Users user;
			int count = 0;
			while (count < 3 && getline(file, line)) {
				size_t delimiterPosition = line.find(delimiter);
				if (delimiterPosition > 0) {
					title = line.substr(0, delimiterPosition);
					value = line.substr(delimiterPosition + 1);

					if (title == "Name") {
						user._name = value;
					}
					else if (title == "Login") {
						user._login = value;
					}
					else if (title == "Password") {
						user._password = value;
					}
					count++;
				}
			}
			allUsers.push_back(user);
		}
		file.close();
	}
}

void Chat::readUsers_DB()
{
	std::cout << "������ ������ �� ������� Users!" << std::endl;
	server.get_Users_DB();
}

int Chat::getReadUsersStatus() {

	std::fstream file(userData, std::ios::in);
	if (!file) {
		return -1;
	}

	std::fstream user_cnt_file(user_count, std::ios::in);
	if (!user_cnt_file) {
		return -1;
	}

	else if (user_cnt_file.is_open()) {
		std::string line;
		int user_cnt = 0;
		getline(user_cnt_file, line);
		user_cnt = stoi(line);
		if (user_cnt == 0) {
			return -1;
		}
	}
	return 1;
}

int Chat::getReadUsersStatus_DB()
{
	if (server.get_Users_DB_status() == 1) {
		return 1;
	}
	else {
		return -1;
	}
}

void Chat::writeUsers() const {

	std::fstream file = std::fstream(userData, std::ios::in | std::ios::out | std::ios::trunc);
	//auto permissions = std::filesystem::perms::group_all | std::filesystem::perms::others_all; // �����, ������� ����� ��������
	//std::filesystem::permissions(userData, permissions, std::filesystem::perm_options::remove);

	for (int i = 0; i < allUsers.size(); i++) {
		file << "Name:" << allUsers[i]._name << "\n";
		file << "Login:" << allUsers[i]._login << "\n";
		file << "Password:" << allUsers[i]._password << "\n";
	}

	file.close();

	std::fstream user_cnt_file = std::fstream(user_count, std::ios::in | std::ios::out | std::ios::trunc);
	//std::filesystem::permissions(user_count, permissions, std::filesystem::perm_options::remove);

	user_cnt_file << allUsers.size();
	user_cnt_file.close();
}

void Chat::writeMessage() const {

	std::fstream file_prvt_msg = std::fstream(private_message, std::ios::in | std::ios::out | std::ios::trunc);
	//auto permissions = std::filesystem::perms::group_all | std::filesystem::perms::others_all; // �����, ������� ����� ��������
	//std::filesystem::permissions(private_message, permissions, std::filesystem::perm_options::remove);

	for (int i = 0; i < allMessage.size(); i++) {
		file_prvt_msg << "Recipient:" << allMessage[i]._recipient << "\n";
		file_prvt_msg << "Sender:" << allMessage[i]._sender << "\n";
		file_prvt_msg << "Message:" << allMessage[i]._message << "\n";
	}
	file_prvt_msg.close();

	std::fstream file_pblc_msg = std::fstream(public_message, std::ios::in | std::ios::out | std::ios::trunc);
	//std::filesystem::permissions(public_message, permissions, std::filesystem::perm_options::remove);

	for (int i = 0; i < allPublicMessage.size(); i++) {
		file_pblc_msg << "Recipient:" << allPublicMessage[i]._recipient << "\n";
		file_pblc_msg << "Sender:" << allPublicMessage[i]._sender << "\n";
		file_pblc_msg << "Message:" << allPublicMessage[i]._message << "\n";
	}

	file_pblc_msg.close();

	std::fstream prvt_msg_cnt_file = std::fstream(prvt_msg_count, std::ios::in | std::ios::out | std::ios::trunc);
	//std::filesystem::permissions(prvt_msg_count, permissions, std::filesystem::perm_options::remove);

	prvt_msg_cnt_file << allMessage.size();
	prvt_msg_cnt_file.close();

	std::fstream pblc_msg_cnt_file = std::fstream(pblc_msg_count, std::ios::in | std::ios::out | std::ios::trunc);
	//std::filesystem::permissions(pblc_msg_count, permissions, std::filesystem::perm_options::remove);

	pblc_msg_cnt_file << allPublicMessage.size();
	pblc_msg_cnt_file.close();
}

void Chat::readPrivateMessage() {

	std::fstream file = std::fstream(private_message, std::ios::in);

	if (!file) {
		server.Write("No file\n");
		return;
	}

	const std::string delimiter = ":";

	if (file.is_open()) {
		std::string line;
		std::string title;
		std::string value;
		int prvt_msg_cnt = 0;

		std::fstream prvt_msg_cnt_file(prvt_msg_count, std::ios::in);
		if (prvt_msg_cnt_file.is_open()) {
			std::string line;
			getline(prvt_msg_cnt_file, line);
			prvt_msg_cnt = stoi(line);
		}
		for (int i = 0; i < prvt_msg_cnt; i++) {
			Message msg;
			int count = 0;
			while (count < 3 && getline(file, line)) {
				size_t delimiterPosition = line.find(delimiter);
				if (delimiterPosition > 0) {
					title = line.substr(0, delimiterPosition);
					value = line.substr(delimiterPosition + 1);

					if (title == "Recipient") {
						msg._recipient = value;
					}
					else if (title == "Sender") {
						msg._sender = value;
					}
					else if (title == "Message") {
						msg._message = value;
					}
					count++;
				}
			}
			allMessage.push_back(msg);
		}
		file.close();
	}
}


void Chat::readPublicMessage() {
	std::fstream file = std::fstream(public_message, std::ios::in);

	if (!file) {
		server.Write("No file");
		return;
	}

	const std::string delimiter = ":";

	if (file.is_open()) {
		std::string line;
		std::string title;
		std::string value;
		int pblc_msg_cnt = 0;

		std::fstream pblc_msg_cnt_file(pblc_msg_count, std::ios::in);
		if (pblc_msg_cnt_file.is_open()) {
			std::string line;
			getline(pblc_msg_cnt_file, line);
			pblc_msg_cnt = stoi(line);
		}
		for (int i = 0; i < pblc_msg_cnt; i++) {
			Message msg;
			int count = 0;
			while (count < 3 && getline(file, line)) {
				size_t delimiterPosition = line.find(delimiter);
				if (delimiterPosition > 0) {
					title = line.substr(0, delimiterPosition);
					value = line.substr(delimiterPosition + 1);

					if (title == "Recipient") {
						msg._recipient = value;
					}
					else if (title == "Sender") {
						msg._sender = value;
					}
					else if (title == "Message") {
						msg._message = value;
					}
					count++;
				}
			}
			allPublicMessage.push_back(msg);
		}
		file.close();
	}
}

void Chat::readPrivateMessage_DB(std::string login)
{
	std::cout << "������ ������ �� ������� private_message!" << std::endl;
	server.get_private_message_DB(login);
}

void Chat::readPublicMessage_DB(std::string login)
{
	std::cout << "������ ������ �� ������� public_message!" << std::endl;
	server.get_public_message_DB(login);
}


void Chat::start() {
	std::string c = "y"; // ������� ������ �� �����
	if (getReadUsersStatus_DB() == 1) { // ���� ���� ���� � ������� � ����� ������������������ �������������,
		// �� ������� ���������� � ����������� ������ ������������ � � ����������� �� ������ ��������� �����������
		readUsers_DB(); // ����� ������������� �� �����, ����� ���� ����� ������ � ������ 
		server.Write("\n\n����� ���������� � ���!\n� �� ��� ���� ������������������ ������������.\n������ ���������������� ��� ������ ������������?(y/n)\n");
		c = server.Read();
	}
	while (c == "y") {
		registration();
		server.Write("\n\n������ ���������������� ��� ������ ������������?(y/n)\n");
		c = server.Read();
	}

	enter(); // �����������
	c = "y";
	while (c != "n") {
		if (getstatus()) { // ��������� ��� �� �������� ����
			char message;
			std::string m;
			server.Write("\n������ ��������� ���������?(y/n)\n");
			c = server.Read();

			if (c == "y")
			{
				server.Write("\n�������� ��� ������������� ���������: 1-private, 2-public:\n");
				m = server.Read();

				if (m == "1") {
					message = '1';
				}

				else if (m == "2") {
					message = '2';
				}

				else {
					message = '3';
				}

				switch (message) {
				case '1':
					sendPrivateMessage();
					break;
				case '2':
					sendPublicMessage();
					break;
				default:
					server.Write("\n������������ ����!\n");
					break;
				}
			}
			if (c == "n") {
				server.Write("\n������ ��������� ���� ��� ������ ������� �������?(y/n)\n");
				c = server.Read();
				if (c == "y") {
					exit();
					enter();
				}
				else {
					break;
				}
			}
		}
		else {
			c = "n";
			server.Write("\n���� �� ��������!\n");
		}
	}
}