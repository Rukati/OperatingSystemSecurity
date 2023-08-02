#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <vector>

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>


using namespace rapidjson;
using namespace std;

class Database {
private:
    Document db;
    string subj;
public:
    Database(const string sub) {
        subj = sub;
        // Èíèöèàëèçàöèÿ ïóñòîãî JSON-îáúåêòà
        db.SetObject();

        ifstream file("database.json");
        if (file.is_open()) {
            string jsonString((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();

            if (!db.Parse(jsonString.c_str()).HasParseError()) {
                //cout << "\033[1;32mÔàéë óñïåøíî çàãðóæåí.\033[0m" << endl;
            }
            else {
                cout << "\033[1;31mÎøèáêà ïðè ðàçáîðå ôàéëà JSON.\033[0m" << endl;
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
            //cout << "\033[1;32mÄàííûå óñïåøíî ñîõðàíåíû â ôàéë.\033[0m" << endl;
        }
        else {
            cout << "\033[1;31mÎøèáêà ïðè ñîõðàíåíèè äàííûõ â ôàéë.\033[0m" << endl;
        }
    }

    void PrintContents() {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        db.Accept(writer);

        cout << "\033[0;34mÑîäåðæèìîå áàçû äàííûõ:\033[0m\n";
        cout << buffer.GetString() << endl;
    }

    void ÑreateSubject() {
        if (db["subjects"].HasMember(subj.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò óæå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }
        Value subjects(kObjectType);
        db["subjects"].AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), subjects, db.GetAllocator());
        Value access_matrix(kObjectType);
        db["access_matrix"].AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), access_matrix, db.GetAllocator());
        cout << "\033[1;32m>>> Ñóáúåêò ñîçäàí.\033[0m" << endl;

        SaveToFile();
    }

    void CreateObject(string obj) {
        if (!db["subjects"].HasMember(subj.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        if (db["objects"].HasMember(obj.c_str())) {
            cout << "\033[1;31m>>> Îáúåêò óæå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        Value objects(kObjectType);
        db["objects"].AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), Value(kArrayType).Move(), db.GetAllocator());
        Value& OwnObj = db["objects"][obj.c_str()];
        OwnObj.PushBack(Value(subj.c_str(), db.GetAllocator()).Move(), db.GetAllocator());

        for (Value::MemberIterator it = db["access_matrix"].MemberBegin(); it != db["access_matrix"].MemberEnd(); ++it) {
            Value& subj = it->name;
            Value& access_matrix = it->value;
            access_matrix.AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), Value(kArrayType).Move(), db.GetAllocator());
        }
        cout << "\033[1;32m>>> Îáúåêò ñîçäàí.\033[0m" << endl;

        AddRight(subj,obj, "r");
        AddRight(subj,obj, "w");
        AddRight(subj,obj, "x");
        AddRight(subj,obj, "o");

        SaveToFile();
    }

    void DeleteObject(string obj) {
        if (!db["objects"].HasMember(obj.c_str())) {
            cout << "\033[1;31m>>> Îáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        bool hasORight = false;

        if (db["access_matrix"].HasMember(subj.c_str()) && db["access_matrix"][subj.c_str()].HasMember(obj.c_str())) {
            const Value& rights = db["access_matrix"][subj.c_str()][obj.c_str()];
            if (rights.IsArray()) {
                for (Value::ConstValueIterator it = rights.Begin(); it != rights.End(); ++it) {
                    //cout << it->GetString() << " ";
                    if (strcmp(it->GetString(), "o") == 0) {
                        hasORight = true;
                        break;
                    }
                }
            }
        }

        if (db["objects"].HasMember(obj.c_str())) {
            const Value& creators = db["objects"][obj.c_str()];
            for (Value::ConstValueIterator it = creators.Begin(); it != creators.End(); ++it) {
                if (it->IsString() && it->GetString() == subj) {
                    hasORight = true;
                }
            }
        }

        if (!hasORight) {
            cout << "\033[1;31m>>> Ñóáúåêò íå èìååò óêàçàííîãî ïðàâà íà îáúåêò.\033[0m" << endl;
            return;
        }

        if (db["objects"].HasMember(obj.c_str())) {
            // Óäàëåíèå îáúåêòà èç ñóáúåêòîâ
            for (Value::MemberIterator it = db["subjects"].MemberBegin(); it != db["subjects"].MemberEnd(); ++it) {
                Value& subject = it->name;
                Value& objects = it->value;
                if (objects.HasMember(obj.c_str())) {
                    objects.RemoveMember(obj.c_str());
                }
            }

            // Óäàëåíèå îáúåêòà èç ìàòðèöû äîñòóïà
            for (Value::MemberIterator it = db["access_matrix"].MemberBegin(); it != db["access_matrix"].MemberEnd(); ++it) {
                Value& subject = it->name;
                Value& access_matrix = it->value;
                if (access_matrix.HasMember(obj.c_str())) {
                    access_matrix.RemoveMember(obj.c_str());
                }
            }

            // Óäàëåíèå îáúåêòà
            db["objects"].RemoveMember(obj.c_str());

            cout << "\033[1;32m>>> Îáúåêò óñïåøíî óäàëåí.\033[0m" << endl;
        }
        else {
            cout << "\033[1;31m>>> Îáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
        }

        SaveToFile();
    }

    void DeleteSubject(string subj2) {
        if (!db["subjects"].HasMember(subj2.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        // Óäàëåíèå ñóáúåêòà èç ìàòðèöû äîñòóïà
        if (db["access_matrix"].HasMember(subj2.c_str())) {
            db["access_matrix"].RemoveMember(subj2.c_str());
        }

        // Óäàëåíèå ñóáúåêòà èç ñïèñêà ñóáúåêòîâ
        db["subjects"].RemoveMember(subj2.c_str());

        cout << "\033[1;32m>>> Ñóáúåêò óäàëåí.\033[0m" << endl;

        SaveToFile();
    }

    void AddRight(string subj2, string obj, string right) {
        if (!db["subjects"].HasMember(subj2.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        if (!db["objects"].HasMember(obj.c_str())) {
            cout << "\033[1;31m>>> Îáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }

        if (right != "r" && right != "w" && right != "x" && right != "o") {
            cout << "\033[1;31m>>> Íåäåéñòâèòåëüíîå ïðàâî.\033[0m" << endl;
            return;
        }

        if (!db["access_matrix"].HasMember(subj2.c_str())) {
            Value access_matrix(kObjectType);
            db["access_matrix"].AddMember(Value(subj2.c_str(), db.GetAllocator()).Move(), access_matrix, db.GetAllocator());
        }

        if (!db["access_matrix"][subj2.c_str()].HasMember(obj.c_str())) {
            Value rights(kArrayType);
            db["access_matrix"][subj2.c_str()].AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), rights, db.GetAllocator());
        }

        Value& rights = db["access_matrix"][subj2.c_str()][obj.c_str()];
        for (Value::ValueIterator it = rights.Begin(); it != rights.End(); ++it) {
            if (it->GetString() == right) {
                cout << "\033[1;31m>>> Ïðàâî óæå ñóùåñòâóåò.\033[0m" << endl;
                return;
            }
        }

        rights.PushBack(Value(right.c_str(), db.GetAllocator()).Move(), db.GetAllocator());
        cout << "\033[1;32m>>> Ïðàâî " << right << " äîáàâëåíî.\033[0m" << endl;

        SaveToFile();
    }

    void DeleteRight(string subj2,string obj, string right) {
        if (!db["subjects"].HasMember(subj2.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }
        if (!db["objects"].HasMember(obj.c_str())) {
            cout << "\033[1;31m>>> Îáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return;
        }
        if (right != "r" && right != "w" && right != "x") {
            cout << "\033[1;31m>>> Íåäåéñòâèòåëüíîå ïðàâî.\033[0m" << endl;
            return;
        }

        if (db["access_matrix"].HasMember(subj2.c_str()) && db["access_matrix"][subj2.c_str()].HasMember(obj.c_str())) {
            Value& rights = db["access_matrix"][subj2.c_str()][obj.c_str()];
            for (Value::ValueIterator it = rights.Begin(); it != rights.End(); ++it) {
                if (it->GetString() == right) {
                    rights.Erase(it);
                    cout << "\033[1;32m>>> Ïðàâî óäàëåíî.\033[0m" << endl;
                    return;
                }
            }
        }
        cout << "\033[1;31m>>> Ñóáúåêò íå èìååò óêàçàííîãî ïðàâà íà îáúåêò.\033[0m" << endl;

        SaveToFile();
    }

    bool ChekSubj()
    {
        if (!db["subjects"].HasMember(subj.c_str())) {
            cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << endl;
            return false;
        }
        else
            return true;
    }

    bool ChekRight(string obj, string right)
    {
        Value& rights = db["access_matrix"][subj.c_str()][obj.c_str()];
        for (Value::ValueIterator it = rights.Begin(); it != rights.End(); ++it) {
            if (it->GetString() == right)
                return true;
        }
        return false;
    }
    
};

class Command {
private:
    Database database;

    void CreateFile(string& subj, string& obj) {
        database.CreateObject(obj);
        //cout << "\033[1;32mÂûïîëíåíà êîìàíäà cmd_create_file: subj=" << subj << ", obj=" << obj << "\033[0m" << endl;
    }

    void DeleteFile(string& subj, string& obj) {
        database.DeleteObject(obj);

        //cout << "\033[1;32mÂûïîëíåíà êîìàíäà cmd_delete_file: subj=" << subj << ", obj=" << obj << "\033[0m" << endl;
    }

    void GrantRight(const string& subj1, const string& subj2, const string& obj, const string& right) {
        if (database.ChekSubj())
            if (database.ChekRight(obj,right))
                database.AddRight(subj2,obj, right);
                //cout << "\033[1;32mÂûïîëíåíà êîìàíäà cmd_grant_right: subj1=" << subj1 << ", subj2=" << subj2 << ", obj=" << obj << ", right=" << right << "\033[0m" << endl;
    }

    string subj;
public:
    Command(const string& sub) : database(sub) {
        subj = sub;
    };

    void Parse(const string& command_str) {
        vector<string> cmd_parts;
        istringstream iss(command_str);
        string part;
        while (iss >> part) {
            cmd_parts.push_back(part);
        }

        if (cmd_parts.empty()) {
            cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
            return;
        }

        string cmd = cmd_parts[0];

        if (cmd == "cmd_create_file") {
            if (cmd_parts.size() < 2) {
                cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
                return;
            }
            string obj = cmd_parts[1];
            CreateFile(subj,obj);
        }
        else if (cmd == "cmd_delete_file") {
            if (cmd_parts.size() < 2) {
                cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
                return;
            }
            string obj = cmd_parts[1];
            DeleteFile(subj,obj);
        }
        else if (cmd == "cmd_delete_subj")
        {
            if (cmd_parts.size() < 2) {
                cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
                return;
            }

            if (subj == "admin")
            {
                string subj = cmd_parts[1];
                database.DeleteSubject(subj);
            }
            else {
                cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
                return;
            }
        }
        else if (cmd == "cmd_grant_right") {
            if (cmd_parts.size() < 4) {
                cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
                return;
            }
            string subj2 = cmd_parts[1];
            string obj = cmd_parts[2];
            string right = cmd_parts[3];
            GrantRight(subj,subj2, obj, right);
        }
        else {
            cout << "\033[1;31m>>> Íåêîððåêòíàÿ êîìàíäà.\033[0m" << endl;
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru");
    while (true) {

        cout << "Ââåäèòå âàøå èìÿ: \033[38;5;207m";
        string name;
        getline(cin, name);
        cout << "\033[0m";

        // Óäàëåíèå ïðîáåëîâ äî è ïîñëå èìåíè
        name.erase(name.begin(), find_if(name.begin(), name.end(), [](unsigned char ch) {
            return !isspace(ch);
            }));
        name.erase(find_if(name.rbegin(), name.rend(), [](unsigned char ch) {
            return !isspace(ch);
            }).base(), name.end());

        if (name == "exit")
            break;

        system("cls");

        if (name.size() > 0) {
            cout << endl;

            while (true)
            {
                Database database(name);
                Command cmd(name);

                if (!database.ChekSubj())
                    database.ÑreateSubject();

                string command_str;

                cout << "Ââåäèòå êîìàíäó: \033[38;5;207m";
                getline(cin, command_str);
                cout << "\033[0m";

                if (command_str == "exit") {
                    system("cls");
                    break;
                }
                else if (command_str == "print")
                {
                    database.PrintContents();
                    continue;
                }
                else if (command_str == "clear")
                {
                    system("cls");
                    ascii_art(name);
                    cout << endl;
                    continue;
                }
                else if (command_str == "list")
                {
                    cout << "\033[0;34mÑïèñîê êîìàíä: \033[0m\n";
                    cout << "\tcmd_create_file obj\n";
                    cout << "\tcmd_delete_file obj\n";
                    cout << "\tcmd_grant_right subj obj right\n";
                    continue;
                }

                cmd.Parse(command_str);

            }
        }
    }

    return 0;
}
