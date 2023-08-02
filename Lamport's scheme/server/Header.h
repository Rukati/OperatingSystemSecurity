#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;
using namespace std;


class Database {
private:
    Document db;
    string subj;

public:
    Database(const string sub) {
        subj = sub;

        ifstream file("database.json");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            if (!db.Parse(buffer.str().c_str()).HasParseError()) {
                //cout << "\033[1;32mФайл успешно загружен.\033[0m" << endl;
            }
            else {
                cout << "\033[1;31mОшибка при разборе файла JSON.\033[0m" << endl;
            }
        }
    }

    void SaveToFile() {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        db.Accept(writer);

        ofstream file("database.json");
        if (file.is_open()) {
            file << buffer.GetString();
            file.close();
            //cout << "\033[1;32mДанные успешно сохранены в файл.\033[0m" << endl;
        }
        else {
            cout << "\033[1;31mОшибка при сохранении данных в файл.\033[0m" << endl;
        }
    }

    void PrintContents() {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        db.Accept(writer);

        cout << "\033[1;32mСодержимое базы данных:\033[0m\n";
        cout << buffer.GetString() << endl;
    }

    void CreateSubject(const string& subj) {
        if (db["login"].HasMember(subj.c_str())) {
            cout << "\033[1;31m>>> Субъект уже существует.\033[0m" << endl;
            return;
        }

        Value subjects(kObjectType);
        Value array(kArrayType);
        array.PushBack(Value().SetInt(1), db.GetAllocator()); // Значение A добавляется в массив
        subjects.AddMember("A", array, db.GetAllocator()); // Массив добавляется в субъект
        db["login"].AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), subjects, db.GetAllocator());
        cout << "\033[1;32m>>> Субъект создан.\033[0m" << endl;

        SaveToFile();
    }

    bool CheckSubject() {
        if (!db["login"].HasMember(subj.c_str())) {
            cout << "\033[1;31m>>> Субъект не существует.\033[0m" << endl;
            return false;
        }
        else 
            return true;
    }

    int GetValueA() {
        if (!CheckSubject()) {
            return 0; // Возвращаем 0, если субъект не существует
        }

        const Value& loginObj = db["login"];
        const Value& subjectObj = loginObj[subj.c_str()];

        if (!subjectObj.HasMember("A") || !subjectObj["A"].IsArray() || subjectObj["A"].Empty()) {
            cout << "\033[1;31m>>> Массив значения A не найден или пустой.\033[0m" << endl;
            return 0; // Возвращаем 0, если массив значения A не найден или пустой
        }

        const Value& arrayA = subjectObj["A"];
        int valueA = arrayA[0].GetInt(); // Получаем первый элемент массива значения A

        return valueA;
    }

    void IncrementValueA() {
        if (!CheckSubject()) {
            cout << "Субъект не существует." << endl;
            return;
        }

        Value& subjectObj = db["login"][subj.c_str()];

        if (!subjectObj.HasMember("A") || !subjectObj["A"].IsArray() || subjectObj["A"].Empty()) {
            cout << "Массив значения A не найден или пустой." << endl;
            return;
        }

        Value& arrayA = subjectObj["A"];
        if (arrayA[0].IsInt()) {
            int valueA = arrayA[0].GetInt();
            valueA++; 
            arrayA[0].SetInt(valueA); 
            cout << "Значение A успешно увеличено на 1." <<"\nA: " << valueA << endl;
            SaveToFile(); 
        }
        else {
            cout << "Некорректное значение A." << endl;
        }
    }

};
