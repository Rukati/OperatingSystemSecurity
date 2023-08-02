#include "Header.h"

#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <vector>
#include <conio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

vector<SOCKET>connections(100);
int Counter = 0;

void Client(int index)
{
	char choice[256];
	recv(connections[index], choice, sizeof(choice), NULL);
	if (atoi(choice) == 1)
	{
		cout << "Клиент хочет войти" << endl;
		char msg_login[256] = " Login: ";
		send(connections[index], msg_login, sizeof(msg_login), NULL);
		char ClientLogin[256];
		recv(connections[index], ClientLogin, sizeof(ClientLogin), NULL);
		if (ClientLogin != "")
		{
			cout << "Login: " << ClientLogin << "\n";
			string saltClient = SQL(ClientLogin,"salt");
			if (saltClient == " ")
			{
				char msg_error[256] = "error";
				send(connections[index], msg_error, sizeof(msg_error), NULL);
				return ;
			}
			char msg_salt[257];
			copy(saltClient.begin(), saltClient.end(), msg_salt);
			//cout << saltClient << "\n";
			send(connections[index], msg_salt, sizeof(msg_salt), NULL);
			char ClientPass[256];
			char msg_pass[256] = " Password: ";
			send(connections[index], msg_pass, sizeof(msg_pass), NULL);
			recv(connections[index], ClientPass, sizeof(ClientPass), NULL);
			cout << "Password: " << ClientPass << "\n";
			if (ClientPass != "")
			{
				// loading
				int barl = 3;
				char msg_load_1[256] = " Loading";
				send(connections[index], msg_load_1, sizeof(msg_load_1), NULL);
				for (int i = 0; i < barl; i++) {
					Sleep(200);
					char msg_load_2[256] = ".";
					send(connections[index], msg_load_2, sizeof(msg_load_2), NULL);
				}
				
				if (SQL(ClientLogin, "hash") == ClientPass)
				{
					char msgTrue[256] = " Авторизация прошла успешно! С возвращением!";
					send(connections[index], msgTrue, sizeof(msgTrue), NULL);
				}
				else
				{
					char msgFalse[256] = " Проверьте данные. Что-то не так ";
					send(connections[index], msgFalse, sizeof(msgFalse), NULL);
				}
				
			}
		}
	}
	if (atoi(choice) == 2)
	{
		cout << "Чел хочет зарегаться" << endl;
	
		char ClientSolt[257];
		recv(connections[index], ClientSolt, sizeof(ClientSolt), NULL);
		cout << ClientSolt;

		char ClientLogin[256];
		char msg_login[256] = " Login: ";
		send(connections[index], msg_login, sizeof(msg_login), NULL);
		recv(connections[index], ClientLogin, sizeof(ClientLogin), NULL);

		char ClientPass[256];
		char msg_pass[256] = " Password: ";
		send(connections[index], msg_pass, sizeof(msg_pass), NULL);
		recv(connections[index], ClientPass, sizeof(ClientPass), NULL);
		cout << ClientPass << "\n";

		string insertSQL = SQL(ClientLogin, "chek");
		if (insertSQL == "ok")
		{
			char msgOK[256] = " Добро пожаловать в семью!";
			send(connections[index], msgOK, sizeof(msgOK), NULL);
			cout << insertSQL << "\n";
			SQL_insert(ClientLogin, ClientSolt, ClientPass);
		}
		else
		{
			char msgNoOK[256] = " Не могу добавить Вас, может Вы уже зарегистрировались и проверяете меня? Или уже есть пользователь с таким логином :( ";
			send(connections[index], msgNoOK, sizeof(msgNoOK), NULL);
			cout << insertSQL << "\n";
		}
	}
}
int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	WSAData wsaData; //структура wsaData
	WORD DLLVersion = MAKEWORD(2, 1);//версия библиотеки
	if (WSAStartup(DLLVersion, &wsaData) != 0) // загрузка библиотеки
	{
		cout << "Error" << endl;
		exit(1);
	}
	SOCKADDR_IN addr; // структура преднозначеная для хранения адреса
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1024);
	addr.sin_family = AF_INET;

	SOCKET slisten = socket(AF_INET, SOCK_STREAM, NULL);
	//  предвариетльно созданный сокет, указатель на структуру, размер структуры.
	bind(slisten, (SOCKADDR*)&addr, sizeof(addr)); // привязали адрес к сокету
	listen(slisten, SOMAXCONN);

	SOCKET newConn;
	for (int i = 0; i < connections.size(); i++)
	{
		newConn = accept(slisten, (SOCKADDR*)&addr, &sizeofaddr);
		if (newConn != 0)
		{
			cout << "Client Connection" << endl;
			char msg[256] = "\t Menu \n 1. Вход \n 2. Регистрация \n Ваш выбор: ";
			send(newConn, msg, sizeof(msg), NULL);

			connections[i] = newConn;
			Counter += 1;

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Client, (LPVOID)(i), NULL, NULL);
		}
		else
		{
			cout << "Error \n";
		}
	}
	return 0;
}