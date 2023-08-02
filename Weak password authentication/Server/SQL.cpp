#include "Header.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <iostream>
#include <cstdlib>

using namespace std;

string SQL(char login[256],string query) {

	string ClientLogin; ClientLogin += login;

	// Создаем экземпляр драйвера для работы с MySQL
	sql::Driver* driver;
	driver = get_driver_instance();

	// Устанавливаем параметры соединения с базой данных
	sql::Connection* con;
	con = driver->connect("localhost", "root", "Rukati256++");

	// Выбираем базу данных
	con->setSchema("bos");

	// Создаем объект класса Statement для выполнения запросов
	sql::Statement* stmt;
	stmt = con->createStatement();

	// Выполняем запрос на чтение данных
	sql::ResultSet* res;
	res = stmt->executeQuery("SELECT * FROM account WHERE UserName='" + ClientLogin + "';");
	
	// Обрабатываем результаты запроса
	while (res->next()) {
		if (res)
		{
			if (query == "salt") {
				string salt = res->getString("Salt");
				return salt;
			}
			else if (query == "hash") {
				string hash = res->getString("Hash");
				return hash;
			}
			if (query == "chek")
			{
				string chek = " no ok";
				return chek;
			}
		}
	}
	if (query == "chek")
	{
		string chek = "ok";
		return chek;
	}
	return " ";
	// Освобождаем ресурсы
	delete res;
	delete stmt;
	delete con;
}