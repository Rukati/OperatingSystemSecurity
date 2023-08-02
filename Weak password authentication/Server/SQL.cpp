#include "Header.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <iostream>
#include <cstdlib>

using namespace std;

string SQL(char login[256],string query) {

	string ClientLogin; ClientLogin += login;

	// Ñîçäàåì ýêçåìïëÿð äðàéâåðà äëÿ ðàáîòû ñ MySQL
	sql::Driver* driver;
	driver = get_driver_instance();

	// Óñòàíàâëèâàåì ïàðàìåòðû ñîåäèíåíèÿ ñ áàçîé äàííûõ
	sql::Connection* con;
	con = driver->connect("localhost", "root", "password");

	// Âûáèðàåì áàçó äàííûõ
	con->setSchema("bos");

	// Ñîçäàåì îáúåêò êëàññà Statement äëÿ âûïîëíåíèÿ çàïðîñîâ
	sql::Statement* stmt;
	stmt = con->createStatement();

	// Âûïîëíÿåì çàïðîñ íà ÷òåíèå äàííûõ
	sql::ResultSet* res;
	res = stmt->executeQuery("SELECT * FROM account WHERE UserName='" + ClientLogin + "';");
	
	// Îáðàáàòûâàåì ðåçóëüòàòû çàïðîñà
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
	// Îñâîáîæäàåì ðåñóðñû
	delete res;
	delete stmt;
	delete con;
}
