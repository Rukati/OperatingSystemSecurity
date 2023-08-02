#include "Header.h"

#include <iostream>
#include <clocale>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <string>
#include <random>

#include <openssl/bn.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

vector<SOCKET>connections(1);
int Counter = 0;
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
string ReadMsg(int index)
{
    // Ïîëó÷àåì ñòðîêó îò ñåðâåðà
    char* buffer = NULL;
    int numBytesReceived = 0;
    int bufferSize = 1024;

    do {
        buffer = (char*)realloc(buffer, bufferSize); // Âûäåëÿåì äèíàìè÷åñêóþ ïàìÿòü äëÿ áóôåðà
        numBytesReceived += recv(connections[index], buffer + numBytesReceived, bufferSize - numBytesReceived, 0);
        if (numBytesReceived == bufferSize) {
            bufferSize *= 2; // Óâåëè÷èâàåì ðàçìåð áóôåðà â äâà ðàçà, åñëè ïðèíÿòûå äàííûå çàïîëíèëè âåñü áóôåð
        }
    } while (numBytesReceived == bufferSize);

    string msg(buffer, buffer + numBytesReceived);

    return msg;
}
//------------------------------
void connect(int index)
{
    KEY key;
   
    BIGNUM* secret_key = key.Secret(16);

    BIGNUM* public_key = key.Public(15);
    string msg = NumberToString(public_key);
    send(connections[index], msg.c_str(), msg.size(), 0);

    BIGNUM* public_key2 = strToBn(ReadMsg(index));
    cout << "\nPublic Key from Bob >>>> " << BN_bn2dec(public_key2) << "\n";

    BIGNUM* partical_key = key.Partical(public_key2);
    msg = NumberToString(partical_key);
    send(connections[index], msg.c_str(), msg.size(), 0);

    BIGNUM* partical_key2 = strToBn(ReadMsg(index));
    cout << "\nPartical Key from Bob >>>> " << partical_key2 << "\n";

    BIGNUM* full_key = key.Full(partical_key2,public_key2);
    string fk = NumberToString(full_key);
    int FullKey = stoi(fk);

    cout << "--------------------------------\n";
    ALICA alica;
    while (true)
    {

        string msgFromBob = ReadMsg(index);

        string decryptedMsg = alica.decrypt_message(msgFromBob, FullKey);
        cout << "\tEncryptedMsg: " << msgFromBob;
        cout << "\nBob: " << decryptedMsg << endl;
        
        string msg;
        cout << "\nYou: ";
        getline(cin, msg);

        string encryptedMsg = alica.encrypt_message(msg, FullKey);
        cout << endl;
        send(connections[index], encryptedMsg.c_str(), encryptedMsg.size(), 0);
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
        cout << "Error" << endl;
        exit(1);
    }
    SOCKADDR_IN addr; // ñòðóêòóðà ïðåäíîçíà÷åíàÿ äëÿ õðàíåíèÿ àäðåñà
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1024);
    addr.sin_family = AF_INET;

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, NULL);
    //  ïðåäâàðèåòëüíî ñîçäàííûé ñîêåò, óêàçàòåëü íà ñòðóêòóðó, ðàçìåð ñòðóêòóðû.
    bind(slisten, (SOCKADDR*)&addr, sizeof(addr)); // ïðèâÿçàëè àäðåñ ê ñîêåòó
    listen(slisten, SOMAXCONN);

    SOCKET newConn;
    for (int i = 0; i < connections.size(); i++)
    {
        newConn = accept(slisten, (SOCKADDR*)&addr, &sizeofaddr);
        if (newConn != 0)
        {
         
            connections[i] = newConn;
            Counter += 1;

            connect(i);
            //CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Client, (LPVOID)(i), NULL, NULL);
        }
        else
        {
            cout << "Error \n";
        }
    }
    return 0;
}
