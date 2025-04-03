#include "Chat.h"

int main()
{
	setlocale(LC_ALL, "");
	Chat chat;
	if (status_connect == "Yes") {
		chat.start();
	}
	else {
		std::cout << "Сервер не подключён" << std::endl;
	}
}