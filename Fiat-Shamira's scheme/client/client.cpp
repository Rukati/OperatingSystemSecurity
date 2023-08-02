#include <winsock2.h>
#include <iostream>
#include <clocale>
#include <string>
#include <algorithm>
#include <cmath>
#include <E:\Library\art.cpp>
#include "sha256.h"
#include <boost/random.hpp>
#include <random>


#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996))

using namespace std;
using namespace boost;
using namespace boost::random;

SOCKET Conn;
//------------------------------
class CLIENT
{
private:
	bool miller_rabin_test(unsigned long long n, int k) {
		if (n < 2) {
			return false;
		}

		if (n == 2 || n == 3) {
			return true;
		}

		if (n % 2 == 0) {
			return false;
		}

		unsigned long long d = n - 1;
		int s = 0;

		while (d % 2 == 0) {
			d /= 2;
			s++;
		}

		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<unsigned long long> dist(2, n - 2);

		for (int i = 0; i < k; i++) {
			unsigned long long a = dist(gen) % (n - 3) + 2; // Случайное число в диапазоне [2, n-2]

			unsigned long long x = pow_mod(a, d, n);
			if (x == 1 || x == n - 1) {
				continue;
			}

			for (int j = 0; j < s - 1; j++) {
				x = pow_mod(x, 2, n);
				if (x == 1) {
					return false;
				}
				if (x == n - 1) {
					break;
				}
			}

			if (x != n - 1) {
				return false;
			}
		}

		return true;
	}

	unsigned long long pow_mod(unsigned long long a, unsigned long long b, unsigned long long m) {
		unsigned long long res = 1;
		while (b > 0) {
			if (b & 1) {
				res = (res * a) % m;
			}
			a = (a * a) % m;
			b >>= 1;
		}
		return res;
	}

	unsigned long long mul_mod(unsigned long long a, unsigned long long b, unsigned long long m) {
		unsigned long long res = 0;
		a = a % m;
		while (b > 0) {
			if (b & 1) {
				res = (res + a) % m;
			}
			a = (a * 2) % m;
			b >>= 1;
		}
		return res;
	}

	unsigned long long r;

public:
	
	int counter = 1;
	unsigned long long secret;
	int rounds = 21;

	string ReadMsg()
	{
		// Получаем строку от сервера
		char* buffer = NULL;
		int numBytesReceived = 0;
		int bufferSize = 1024;

		do {
			buffer = (char*)realloc(buffer, bufferSize); // Выделяем динамическую память для буфера
			numBytesReceived += recv(Conn, buffer + numBytesReceived, bufferSize - numBytesReceived, 0);
			if (numBytesReceived == bufferSize) {
				bufferSize *= 2; // Увеличиваем размер буфера в два раза, если принятые данные заполнили весь буфер
			}
		} while (numBytesReceived == bufferSize);

		string msg(buffer, buffer + numBytesReceived);

		return msg;
	}
	
	unsigned long long createV(unsigned long long N)
	{
		boost::random::mt19937 gen(static_cast<unsigned long long>(time(nullptr)));
		boost::random::uniform_int_distribution<unsigned long long> dist(0, (1LL << 31) - 1);

		do {
			secret = dist(gen);
		} while (!miller_rabin_test(secret, 64));

		unsigned long long v = mul_mod(secret, secret, N);

		return v;
	}

	unsigned long long createX(unsigned long long N, unsigned long long V)
	{
		boost::random::mt19937 gen(static_cast<unsigned long long>(time(nullptr)));
		boost::random::uniform_int_distribution<unsigned long long> dist(0, (1LL << 31) - 1);

		unsigned long long x;
		do {
			r = dist(gen);
			x = mul_mod(r, r, N);
		} while (x == V);

		return x;
	}
	
	unsigned long long createY(int e, unsigned long long n)
	{
		switch (e)
		{
			case(1):
			{
				return r * static_cast<unsigned long long>(pow(secret, e)) % n;
			}	
			case(0):
			{
				return r;
			}
		}
	}

};


//------------------------------
void Server()
{
	CLIENT user;
	
	unsigned long long N = stoll( user.ReadMsg());
	unsigned long long V = user.createV(N);
	//cout << "V: " << V;
	send(Conn, to_string(V).c_str(), to_string(V).size(), 0);

	//user.secret = 412412441242334;

	while (user.counter != user.rounds) {
		unsigned long long X = user.createX(N, V);
		//cout << "\nX: " << X;
		send(Conn, to_string(X).c_str(), to_string(X).size(), 0);
		int E = stoi(user.ReadMsg());
		//cout << "\nE: " << E;
		unsigned long long y = user.createY(E, N);
		//cout << "\nY: " << y;
		send(Conn, to_string(y).c_str(), to_string(y).size(), 0);
		string verific = user.ReadMsg();
		cout << "\n" << user.counter << " >>> " << verific;
		user.counter++;
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

	//ascii_art("WELCOM!");
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Server, NULL, NULL, NULL);
	Server();
	
	return 0;
}