#include "Header.h"

#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <string>
#include <algorithm>
#include <cmath>

#include <random>
#include <openssl/bn.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996))

using namespace std;

SOCKET Conn;
//------------------------------
BIGNUM* strToBn(const std::string& str) {
	BIGNUM* bn = BN_new();
	BN_dec2bn(&bn, str.c_str());
	return bn;
}
//------------------------------
string NumberToString(const BIGNUM* num) {
	char* str = BN_bn2dec(num);
	std::string result(str);
	OPENSSL_free(str);
	return result;
}
//------------------------------
string ReadMsg()
{
	// Ïîëó÷àåì ñòðîêó îò ñåðâåðà
	char* buffer = NULL;
	int numBytesReceived = 0;
	int bufferSize = 1024;

	do {
		buffer = (char*)realloc(buffer, bufferSize); // Âûäåëÿåì äèíàìè÷åñêóþ ïàìÿòü äëÿ áóôåðà
		numBytesReceived += recv(Conn, buffer + numBytesReceived, bufferSize - numBytesReceived, 0);
		if (numBytesReceived == bufferSize) {
			bufferSize *= 2; // Óâåëè÷èâàåì ðàçìåð áóôåðà â äâà ðàçà, åñëè ïðèíÿòûå äàííûå çàïîëíèëè âåñü áóôåð
		}
	} while (numBytesReceived == bufferSize);

	string msg(buffer, buffer + numBytesReceived);
	return msg;
}
//------------------------------
void connect()
{
	KEY key;
	
	BIGNUM* secret_key = key.Secret(16);
	BIGNUM* public_key = key.Public(15);

	BIGNUM* public_key2 = strToBn(ReadMsg());
	cout << "\nPublic Key from Alica >>>> " << BN_bn2dec(public_key2) << "\n";

	string msg = NumberToString(public_key);
	send(Conn, msg.c_str(), msg.size(), 0);

	BIGNUM* partical_key = key.Partical(public_key2);
	msg = NumberToString(partical_key);
	send(Conn, msg.c_str(), msg.size(), 0);

	BIGNUM* partical_key2 = strToBn(ReadMsg());
	cout << "\nPartical Key from Alica >>>> " << BN_bn2dec(partical_key2) << "\n";
	 
	BIGNUM* full_key = key.Full(partical_key2);
	string fk = NumberToString(full_key);
	int FullKey = stoi(fk);

	cout << "--------------------------------";

	BOB bob;
	while (true)
	{
		string msg;
		cout << "\nYou: ";
		getline(cin, msg);

		string encryptedMsg = bob.encrypt_message(msg, FullKey);
		send(Conn, encryptedMsg.c_str(), encryptedMsg.size(), 0);

		string msgFromAlica = ReadMsg();
		cout << "\n\tEncryptedMsg: " << msgFromAlica;
		
		string decryptedMsg = bob.decrypt_message(msgFromAlica, FullKey);
		cout << "\nAlica: " << decryptedMsg << endl;
	}
}
//------------------------------

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	WSAData wsaData; //ñòðóêòóðà wsaData
	WORD DLLVersion = MAKEWORD(2, 1);//âåðñèÿ áèáëèîòåêè
	if (WSAStartup(DLLVersion, &wsaData) != 0) // çàãðóçêà áèáëèîòåêè
	{
		cout << "Error WSAData" << endl;
		exit(1);
	}
	SOCKADDR_IN addr; // ñòðóêòóðà ïðåäíîçíà÷åíàÿ äëÿ õðàíåíèÿ àäðåñà
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1024);
	addr.sin_family = AF_INET;

	Conn = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(Conn, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		cout << "Error Connect" << endl;
		return 1;
	}

	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Server, NULL, NULL, NULL);
	connect();

	return 0;
}
