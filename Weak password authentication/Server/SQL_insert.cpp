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

	// ������� ��������� �������� ��� ������ � MySQL
	sql::Driver* driver;
	driver = get_driver_instance();

	// ������������� ��������� ���������� � ����� ������
	sql::Connection* con;
	con = driver->connect("localhost", "root", "Rukati256++");

	// �������� ���� ������
	con->setSchema("bos");

	// ������� ������ ������ Statement ��� ���������� ��������
	sql::Statement* stmt;
	stmt = con->createStatement();

	// ��������� ������ �� ������ ������
	sql::ResultSet* res;
	res = stmt->executeQuery("INSERT INTO `account` (`id`, `UserName`, `Hash`, `Salt`) VALUES (NULL, '"+ClientLogin+"', '"+ClientHash+"', '"+ ClientSolt +"')");

	// ����������� �������
	delete res;
	delete stmt;
	delete con;
}