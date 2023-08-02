#define _CRT_SECURE_NO_WARNING

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Header.h"

using namespace std;


int main(){
    setlocale(LC_ALL, "ru");

    Database database;
    database.ConvertDatabaseToGraph();

    while (true)
    {
        Command cmd(database);
        string command_str;

        cout << BLUE_COLOR << "Введите команду: " << RESET_COLOR;
        getline(cin, command_str);

        if (command_str == "exit") {
            system("cls");
            break;
        }
        else if (command_str == "clear")
        {
            system("cls");
            continue;
        }
        else if (command_str == "print")
        {
            database.ShowGraphDatabase();
            continue;
        }

        cmd.Parse(command_str);
    }
    
    return 0;
}