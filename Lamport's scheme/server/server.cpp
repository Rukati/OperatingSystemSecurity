#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <vector>
#include <conio.h>
#include <ctime>
#include <string>
#include "sha256.h"

#include "Header.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

vector<SOCKET>connections(100);
int Counter = 0;

int N = 2500;
//------------------------------
string HashClient;
string User;
int A;
string strA;
//------------------------------
string ReadMsg(int index)
{
	// Получаем строку от сервера
	char* buffer = NULL;
	int numBytesReceived = 0;
	int bufferSize = 1024;

	do {
		buffer = (char*)realloc(buffer, bufferSize); // Выделяем динамическую память для буфера
		numBytesReceived += recv(connections[index], buffer + numBytesReceived, bufferSize - numBytesReceived, 0);
		if (numBytesReceived == bufferSize) {
			bufferSize *= 2; // Увеличиваем размер буфера в два раза, если принятые данные заполнили весь буфер
		}
	} while (numBytesReceived == bufferSize);

	string msg(buffer, buffer + numBytesReceived);

	return msg;
}
//------------------------------
void HashNTimes()
{
	for (int i = 0; i < N - A + 1; i++)
		HashClient = sha256(HashClient);
	//cout << "\nHash N-A+1 Times = " << HashClient << " A:" << A << endl;
}
//------------------------------
void Client(int index)
{	
	while (true) {
		string otvet = ReadMsg(index);

		if (otvet[otvet.size()-1] == 'H') {
			otvet.pop_back();
			User = otvet;
			HashClient = otvet;
			Database database(HashClient);
			//cout << "HashClienta: " << HashClient;
			if (!database.CheckSubject()) {
				database.CreateSubject(HashClient);
				string msg = "RHello man, im your register!! Welcome!";
				send(connections[index], msg.c_str(), msg.size(), NULL);
			}
			//------------------------------
			A = database.GetValueA();
			cout << "A:" << A;
			HashNTimes();
			strA = 'A' + to_string(A);
		}

		if (otvet == "stop")
		{

			cout << " => last pass failed ; no more attempts ";
			break;
		}

		if (otvet == "ready") {
			int countSus = 0;
			//			 number A
			//------------------------------
			send(connections[index], strA.c_str(), strA.size(), NULL);
			//------------------------------
			string Hash_N_A_Client = ReadMsg(index);
			//------------------------------
			cout << "\nN-A: " << Hash_N_A_Client;
			Hash_N_A_Client = sha256(Hash_N_A_Client); // hash(N-A)
			cout << "\nHash(N-A) :" << Hash_N_A_Client;
			cout << "\nN-A + 1:" << HashClient;
			//------------------------------
			if (Hash_N_A_Client == HashClient) { //
				char sus[256] = "S Successfully!";
				send(connections[index], sus, sizeof(sus), NULL);
				cout << " = Accepted\n";
				Database database(User);
				database.IncrementValueA();
				HashNTimes();
			}
			else {
				char Nosus[256] = "N Fail!";
				send(connections[index], Nosus, sizeof(Nosus), NULL);
				cout << " = Failed\n";
			}
			//------------------------------
			char repeat[256] = "M";
			send(connections[index], repeat, sizeof(repeat), NULL);
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
			char msg[256] = "M\n\tAuthentication ";
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