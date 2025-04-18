#pragma once
#include <iostream>
#include <string>
struct Message
{
	std::string _message; // сообщение
	std::string _recipient; // получатель
	std::string _sender; // отправитель

	Message() // конструткор
	{
		_message = "-";
		_recipient = "-";
	}
	~Message() // деструктор
	{

	}
	void setMessage(std::string text) //  ввод сообщения
	{
		/*if (std::cin.peek() == '\n')
		{
			std::cin.get();// убирает оставшийся в потоке символ перехода на новую строку
		}
		std::getline(std::cin, _message);*/

		_message = text;
	}

	void getMessage() // вывод сообщения
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
