#include "Header.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <iostream>
#include <cstdlib>

using namespace std;

string SQL(char login[256],string query) {

	string ClientLogin; ClientLogin += login;

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
	res = stmt->executeQuery("SELECT * FROM account WHERE UserName='" + ClientLogin + "';");
	
	// ������������ ���������� �������
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
	// ����������� �������
	delete res;
	delete stmt;
	delete con;
}