#pragma once
#include <iostream>
#include <string>
struct Message
{
	std::string _message; // ���������
	std::string _recipient; // ����������
	std::string _sender; // �����������

	Message() // �����������
	{
		_message = "-";
		_recipient = "-";
	}
	~Message() // ����������
	{

	}
	void setMessage(std::string text) //  ���� ���������
	{
		/*if (std::cin.peek() == '\n')
		{
			std::cin.get();// ������� ���������� � ������ ������ �������� �� ����� ������
		}
		std::getline(std::cin, _message);*/

		_message = text;
	}

	void getMessage() // ����� ���������
	{
		std::cout << _message;
	}

	std::string getSender()
	{
		return _sender;
	}

	std::string getRecipient()
	{
		return _recipient;
	}

	std::string getText()
	{
		return _message;
	}
};
