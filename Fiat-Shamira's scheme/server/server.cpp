#include <iostream>
#include <clocale>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <string>
#include <random>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

vector<SOCKET>connections(100);
int Counter = 0;

class SERVER {
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

    int bit_e;
    unsigned long long n;

public:
    int createE()   
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);

        bit_e = dist(gen);
        return bit_e;
    }

    unsigned long long gen_N()
    {
        random_device rd;
        mt19937_64 gen(rd());

        uniform_int_distribution<unsigned long long> dist(0, (1LL << 31) - 1); // ограничение на значения p и q

        unsigned long long p, q;

        do {
            p = dist(gen);
            p |= 1;
        } while (!miller_rabin_test(p, 64));

        do {
            q = dist(gen);
            q |= 1;
        } while (!miller_rabin_test(q, 64));
        n = p * q;
        return n;
    }

    unsigned long long read_Msg(int index)
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

        return stoull(msg);
    }
    
    string verification(unsigned long long y, unsigned long long x, unsigned long long v) {
        unsigned long long verific = pow_mod(v, bit_e, n);
        verific = mul_mod(x, verific, n);

        if (mul_mod(y, y, n) == verific) {
            return "Accept";
        }
        else {
            return "Fail";
        }
    }
};

void Client(int index)
{
	SERVER serv;

	unsigned long long N = serv.gen_N();
	//cout << "N: " << N;
	send(connections[index], to_string(N).c_str(), to_string(N).size(), 0);

    unsigned long long V = serv.read_Msg(index);
    //cout << "\nV: " << V;
    while (true) {
        unsigned long long X = serv.read_Msg(index);
        //cout << "\nX: " << X;   
        int E = serv.createE();
        send(connections[index], to_string(E).c_str(), to_string(E).size(), 0);

        //cout << "\nE: " << E;
        unsigned long long y = serv.read_Msg(index);
        //cout << "\nY: " << y;
        string verific = serv.verification(y, X, V);
        send(connections[index], verific.c_str(), verific.size(), 0);
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

			connections[i] = newConn;
			Counter += 1;

            Client(i);
			//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Client, (LPVOID)(i), NULL, NULL);
		}
		else
		{
			cout << "Error \n";
		}
	}
	return 0;
}