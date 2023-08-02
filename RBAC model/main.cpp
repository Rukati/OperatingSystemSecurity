#include <iostream>
#include "Class.h"
#include "ColorText.h"

int main()
{
	setlocale(LC_ALL, "ru");
	Access access;
	access.PrintData();
	std::cout << "----------------------------" << std::endl;
	access.createNewRole();
	std::cout << "----------------------------" << std::endl;
	return 0;
}