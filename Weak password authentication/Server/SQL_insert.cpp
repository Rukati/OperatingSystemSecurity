#include "Header.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <iostream>
#include <cstdlib>

using namespace std;

void SQL_insert(char login[256],char solt[257],char hash[256]) 
{

	string ClientLogin; ClientLogin += login;
	string ClientSolt; ClientSolt += solt;
	string ClientHash; ClientHash += hash;

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
	res = stmt->executeQuery("INSERT INTO `account` (`id`, `UserName`, `Hash`, `Salt`) VALUES (NULL, '"+ClientLogin+"', '"+ClientHash+"', '"+ ClientSolt +"')");

	// Освобождаем ресурсы
	delete res;
	delete stmt;
	delete con;
}