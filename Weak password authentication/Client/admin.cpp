#include "Header.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <iostream>

using namespace std;

void admin()
{

	cout << "\n \n \n В какую из таблиц хотели бы зайти?";
	cout << "\n \t 1. Contract";
	cout << "\n \t 2. Customer";
	cout << "\n \t 3. Worker_Org \n \t \n Выбор: ";
	int choice;
	cin >> choice;

	switch (choice)
	default:
	{
		cout << " Еще не готово!";
	}

}