#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <string>
#include <algorithm>

//#include <C:\Users\mega-\OneDrive\Рабочий стол\Library\art.cpp>
#include "sha256.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996))

using namespace std;

SOCKET Conn;
//------------------------------
int N = 2500;
string Hash;
int attempt = 2;
//------------------------------
void Server()
{
	char otvet[256];
	recv(Conn, otvet, sizeof(otvet), NULL);
	if (attempt != 0) {
		switch (otvet[0])
		{
		case ('A'): // number A;
		{
			otvet[0] = ' ';
			int A = atoi(otvet);
			//cout << "\nA: " << A;
			//------------------------------
			for (int i = 0; i < N - A; i++) {
				Hash = sha256(Hash);
			}
			//cout << "\nN-A Hash: " << Hash;
			//------------------------------
			send(Conn, Hash.c_str(), Hash.size(), NULL);
			//------------------------------
			break;
		}
		case('M'): // Menu
		{
			otvet[0] = ' ';
			cout << otvet;
			char ClientLog[256];
			char ClientPass[256];
			//------------------------------
			cout << "\n Login: \033[38;5;207m";
			cin.getline(ClientLog, sizeof(ClientLog));
			cout << "\033[0m";
			cout << " Password: \033[38;5;207m ";
			cin.getline(ClientPass, sizeof(ClientPass));
			cout << "\033[0m";
			//------------------------------
			string ClLog; ClLog += ClientLog;
			string ClPass; ClPass += ClientPass;
			Hash = sha256(ClLog + ClPass); Hash += 'H';
			//------------------------------
			send(Conn, Hash.c_str(), Hash.size(), NULL);
			Hash.pop_back();
			string ready = "ready";
			send(Conn, ready.c_str(), ready.size(), NULL);
			//
			break;
		}
		case('S'): // Successfully
		{
			otvet[0] = ' ';
			cout << otvet << " \n";
			break;
		}
		case('N'): // No Successfully
		{
			otvet[0] = ' ';
			cout << otvet << " \n";
			attempt--;
			if (attempt == 0) {
				cout << " Number of attempts completed\n";
				char stop[256] = "stop";
				send(Conn, stop, sizeof(stop), NULL);
			}
			break;
		}
		case('R'): // Register
		{
			otvet[0] = ' ';
			cout << otvet << " \n";
			break;
		}
		default:
			break;
		}
	}
}
//------------------------------
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

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Server, NULL, NULL, NULL);
	Server();
	
	
	while (true)
	{
		Server();
	}
	
	return 0;
}