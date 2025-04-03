#pragma once
#include <iostream>

class Users
{
public:
	std::string _name; // имя
	std::string _login; // логин
	std::string _password; // пароль
	Users(); // конструктор
	~Users() = default;// деструктор
	void setUser(); // метод запрашивает данные пользователя
	void setLogin(std::string lgn);
	void setPassword(std::string pswd);
	void setName(std::string nm);
	std::string getLogin(); // метод возвращает логин пользователя
	std::string getPassword(); // метод возвращает пароль пользователя
	std::string getName(); // метод возвращает имя пользователя
	friend std::ostream& operator<<(std::ostream& output, const Users& user); // перегрузка оператора вывода
	bool operator==(const Users& user); // перегрузка оператора вывода
};