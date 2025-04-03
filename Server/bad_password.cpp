#include "bad_password.h"

const char* BadPassword::what() const noexcept
{
	return "\nНеправильный пароль!\nХотите повторить ввод?(y/n):";
}