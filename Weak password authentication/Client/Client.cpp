#include "Header.h"

#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996))

using namespace std;

SOCKET Conn;
bool flag = true;

void Server()
{
	
	char msg[257];
	recv(Conn, msg, sizeof(msg), NULL);
	string hash;
	string solt;
	if (strlen(msg) == 256)
	{
		solt += msg;
		recv(Conn, msg, sizeof(msg), NULL);
		cout << msg;
	}
	else
		if (strcmp(msg, "error") == 0) {
			cout << "          ^ \n Такого логина не существует. Советуем зарегистрироваться!";
			return ;
		}
		else
			cout << msg;
	if (strcmp(msg, " Loading") == 0)
	{
		flag = false;
	}
	if (flag) 
	{
		char msg_otvet[256];
		cin.getline(msg_otvet, sizeof(msg_otvet));

		if (strcmp(msg_otvet,"2") == 0)
		{
			send(Conn, msg_otvet, sizeof(msg_otvet), NULL);
			solt = Solt();
			char msg_solt[257];
			copy(solt.begin(), solt.end(), msg_solt);
			msg_solt[solt.length()] = 0;
			send(Conn, msg_solt, sizeof(msg_solt), NULL);
			recv(Conn, msg, sizeof(msg), NULL);
			cout << msg;
		}

		if (strcmp(msg, " Password: ") == 0)
		{
			hash = Hash(msg_otvet, solt);
			copy(hash.begin(), hash.end(), msg_otvet);
			msg_otvet[hash.length()] = 0;
		}
		send(Conn, msg_otvet, sizeof(msg_otvet), NULL);
	}

	if (strcmp(msg, " Авторизация прошла успешно! С возвращением!") == 0)
	{
		admin();
	}
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	WSAData wsaData; //структура wsaData
	WORD DLLVersion = MAKEWORD(2, 1);//версия библиотеки
	if (WSAStartup(DLLVersion, &wsaData) != 0) // загрузка библиотеки
	{
		cout << "Error WSAData" << endl;
		exit(1);
	}
	SOCKADDR_IN addr; // структура преднозначеная для хранения адреса
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1024);
	addr.sin_family = AF_INET;

	Conn = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(Conn, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		cout << "Error Connect" << endl;
		return 1;
	}
	cout << "Connect" << endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Server, NULL, NULL, NULL);
	
	while (true)
	{
		Server();
	}
	return 0;
}