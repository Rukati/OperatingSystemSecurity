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
	res = stmt->executeQuery("INSERT INTO `account` (`id`, `UserName`, `Hash`, `Salt`) VALUES (NULL, '"+ClientLogin+"', '"+ClientHash+"', '"+ ClientSolt +"')");

	// Îñâîáîæäàåì ðåñóðñû
	delete res;
	delete stmt;
	delete con;
}
