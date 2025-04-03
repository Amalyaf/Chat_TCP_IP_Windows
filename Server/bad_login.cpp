#include "bad_login.h"

const char* BadLogin::what() const noexcept
{
	return "\nПользователь не найден!\nХотите повторить ввод?(y/n): ";
}