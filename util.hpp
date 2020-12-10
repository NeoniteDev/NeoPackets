#include <iostream>
#include <thread>
#include <Windows.h>
#include <string>
#include <sstream>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

template <typename ...Args>
void printd(Args&& ...args)
{
	std::stringstream ss;
	(ss << ... << args);
	std::string s = ss.str();
	if (strstr(s.c_str(), "[+]"))
	{
		SetConsoleTextAttribute(hConsole, 2);
	}
	else if (strstr(s.c_str(), "[x]"))
	{
		SetConsoleTextAttribute(hConsole, 4);
	}
	else if (strstr(s.c_str(), "[=]"))
	{
		SetConsoleTextAttribute(hConsole, 3);
	}
	else if (strstr(s.c_str(), "[>]"))
	{
		SetConsoleTextAttribute(hConsole, 6);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 7);
	}
	(std::cout << ... << args);
}