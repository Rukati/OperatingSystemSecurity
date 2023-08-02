#pragma once
#include <iostream>
#include <vector>
#include <sstream>

#include "Color.cpp"

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>

using namespace std;
using namespace rapidjson;

#define accessGraph "accessGraph.json"

vector<pair<string, vector<pair<string, vector<string>>>>> graph;

class Database {
private:
    Document db;
public:

    Database()
    {
        // Инициализация пустого JSON-объекта
        db.SetObject();

        ifstream file(accessGraph);
        if (file.is_open()) {
            string jsonString((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();

            if (!db.Parse(jsonString.c_str()).HasParseError()) {
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

        ofstream file(accessGraph);
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

        cout << "\033[0;34mСодержимое базы данных:\033[0m\n";
        cout << buffer.GetString() << endl;
    }

    void ShowGraphDatabase() {
        for (const auto& entry : graph) {
            const string& node = entry.first;
            const vector<pair<string, vector<string>>>& linkedObjects = entry.second;
            cout << node << ": ";
            for (const auto& link : linkedObjects) {
                const string& linkedObj = link.first;
                const vector<string>& weights = link.second;
                for (const auto& weight : weights) {
                    cout << linkedObj << "(" << weight << ") ";
                }
            }
            cout << endl;
        }
    }

    void ConvertDatabaseToGraph() {
        graph.clear();

        // Обработка связей субъектов
        if (db.HasMember("subjects")) {
            const Value& subjects = db["subjects"];
            for (auto it = subjects.MemberBegin(); it != subjects.MemberEnd(); ++it) {
                const string& subj = it->name.GetString();
                const Value& subjObj = it->value;

                if (subjObj.IsArray()) {
                    vector<pair<string, vector<string>>> linkedObjects;
                    for (SizeType i = 0; i < subjObj.Size(); ++i) {
                        const Value& linkedObj = subjObj[i];
                        if (linkedObj.IsObject()) {
                            for (auto linkIt = linkedObj.MemberBegin(); linkIt != linkedObj.MemberEnd(); ++linkIt) {
                                const string& obj = linkIt->name.GetString();
                                const Value& weights = linkIt->value;
                                if (weights.IsArray()) {
                                    vector<string> weightList;
                                    for (SizeType j = 0; j < weights.Size(); ++j) {
                                        const string& weight = weights[j].GetString();
                                        weightList.push_back(weight);
                                    }
                                    linkedObjects.push_back(make_pair(obj, weightList));
                                }
                            }
                        }
                    }
                    graph.push_back(make_pair(subj, linkedObjects));
                }
            }
        }

        // Обработка связей объектов
        if (db.HasMember("objects")) {
            const Value& objects = db["objects"];
            for (auto it = objects.MemberBegin(); it != objects.MemberEnd(); ++it) {
                const string& obj = it->name.GetString();
                const Value& objLinks = it->value;

                if (objLinks.IsArray()) {
                    vector<pair<string, vector<string>>> linkedObjects;
                    for (SizeType i = 0; i < objLinks.Size(); ++i) {
                        const Value& linkedObj = objLinks[i];
                        if (linkedObj.IsObject()) {
                            for (auto linkIt = linkedObj.MemberBegin(); linkIt != linkedObj.MemberEnd(); ++linkIt) {
                                const string& linkedObjName = linkIt->name.GetString();
                                const Value& weights = linkIt->value;
                                if (weights.IsArray()) {
                                    vector<string> weightList;
                                    for (SizeType j = 0; j < weights.Size(); ++j) {
                                        const string& weight = weights[j].GetString();
                                        weightList.push_back(weight);
                                    }
                                    linkedObjects.push_back(make_pair(linkedObjName, weightList));
                                }
                            }
                        }
                    }
                    graph.push_back(make_pair(obj, linkedObjects));
                }
            }
        }
    }

    bool DFS(const string& startNode, const string& target, vector<string>& visited, const vector<string>& rights) {
        visited.push_back(startNode);

        if (startNode == target) {
            return true;
        }

        for (const auto& entry : graph) {
            const string& node = entry.first;
            const vector<pair<string, vector<string>>>& linkedObjects = entry.second;

            if (node == startNode) {
                for (const auto& link : linkedObjects) {
                    const string& linkedObj = link.first;
                    const vector<string>& weights = link.second;

                    // Проверяем вес связи
                    for (const auto& weight : weights) {
                        if (find(rights.begin(), rights.end(), weight) != rights.end()) {
                            // Если связанный объект еще не посещен, вызываем DFS для него
                            if (find(visited.begin(), visited.end(), linkedObj) == visited.end()) {
                                if (DFS(linkedObj, target, visited, rights)) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    bool HasEdge(const string& node, const string& target, const vector<string>& rights) {
        auto it = find_if(graph.begin(), graph.end(), [&](const pair<string, vector<pair<string, vector<string>>>>& p) {
            return p.first == node;
            });

        if (it != graph.end()) {
            const vector<pair<string, vector<string>>>& linkedObjects = it->second;
            for (const auto& link : linkedObjects) {
                const string& linkedObj = link.first;
                const vector<string>& weights = link.second;
                for (const auto& weight : weights) {
                    if (linkedObj == target && find(rights.begin(), rights.end(), weight) != rights.end()) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void CreateSubject(string subj) {
        if (db["subjects"].HasMember(subj.c_str())) {
            return;
        }
        Value subjects(kObjectType);
        db["subjects"].AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), Value(kArrayType).Move(), db.GetAllocator());
        SaveToFile();
    }

    void CreateObject(string obj) {
        if (db["objects"].HasMember(obj.c_str())) {
            return;
        }

        Value objects(kObjectType);
        db["objects"].AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), Value(kArrayType).Move(), db.GetAllocator());
        SaveToFile();
    }

    void AddVertex(const string& type, const string& vertex, const string& newVertex, const vector<string>& rights) {
        if (type == "subject")
            CreateSubject(newVertex);
        else if (type == "object")
            CreateObject(newVertex);

        string section;
        if (db["objects"].HasMember(vertex.c_str())) {
            section = "objects";
        }

        if (db["subjects"].HasMember(vertex.c_str())) {
            section = "subjects";
        }

        Value& subjectArray = db[section.c_str()][vertex.c_str()];

        // Проверяем наличие нового элемента в массиве субъекта
        bool newVertexExists = false;
        for (SizeType i = 0; i < subjectArray.Size(); i++) {
            Value& existingValue = subjectArray[i];
            for (Value::MemberIterator it = existingValue.MemberBegin(); it != existingValue.MemberEnd(); ++it) {
                const string& obj = it->name.GetString();
                if (obj == newVertex) {
                    newVertexExists = true;
                    // Проверяем наличие правила в связи и добавляем право, если его нет
                    Value& ruleArray = it->value;
                    bool rightExists = false;
                    for (SizeType j = 0; j < ruleArray.Size(); j++) {
                        const string& existingRight = ruleArray[j].GetString();
                        if (find(rights.begin(), rights.end(), existingRight) != rights.end()) {
                            rightExists = true;
                            break;
                        }
                    }
                    if (!rightExists) {
                        for (const auto& right : rights) {
                            Value newRight;
                            newRight.SetString(right.c_str(), db.GetAllocator());
                            ruleArray.PushBack(newRight.Move(), db.GetAllocator());
                        }
                        SaveToFile();
                    }
                    break;
                }
            }
        }

        if (!newVertexExists) {
            // Создаем новую связь и добавляем права
            Value newVertexObject(kObjectType);
            Value newRuleArray(kArrayType);
            for (const auto& right : rights) {
                Value newRight;
                newRight.SetString(right.c_str(), db.GetAllocator());
                newRuleArray.PushBack(newRight.Move(), db.GetAllocator());
            }
            newVertexObject.AddMember(Value(newVertex.c_str(), db.GetAllocator()).Move(), newRuleArray.Move(), db.GetAllocator());
            subjectArray.PushBack(newVertexObject.Move(), db.GetAllocator());

            SaveToFile();
        }
    }

    void RemoveEdge(const string& fromVertex, const string& toVertex) {

        if (!HasEdge(fromVertex, toVertex, {"r"})) {
            cout << RED_COLOR << "I can't do it.\n" << RESET_COLOR;
            return;
        }

        Value& subjects = db["subjects"];
        for (Value::MemberIterator subjIt = subjects.MemberBegin(); subjIt != subjects.MemberEnd(); ++subjIt) {
            Value& subjObj = subjIt->value;
            for (Value::ValueIterator fileIt = subjObj.Begin(); fileIt != subjObj.End();) {
                Value& fileObj = *fileIt;
                if (fileObj.HasMember(toVertex.c_str())) {
                    fileObj.RemoveMember(toVertex.c_str());
                }
                else {
                    ++fileIt;
                }
            }
        }

        Value& objects = db["objects"];
        for (Value::MemberIterator objIt = objects.MemberBegin(); objIt != objects.MemberEnd(); ++objIt) {
            Value& objLinks = objIt->value;
            for (Value::ValueIterator linkIt = objLinks.Begin(); linkIt != objLinks.End();) {
                Value& linkObj = *linkIt;
                if (linkObj.HasMember(toVertex.c_str())) {
                    linkObj.RemoveMember(toVertex.c_str());
                }
                else {
                    ++linkIt;
                }
            }
        }
        cout << GREEN_COLOR << "Im deleted" << RESET_COLOR << endl;
        SaveToFile();
    }

};

class Command {
private:
    Database& database;  // Ссылка на объект базы данных
    vector<string> visit;

    //                             Право                К Кому                От Кого                 На Что
    void commandTake(const string& Right, const string& toWhom, const string& fromWhom, const string& onWhat) {
        if (database.HasEdge(fromWhom, onWhat, { Right, "a" })) {
            if (database.DFS(toWhom, fromWhom, visit, { "t", "a" })) {
                cout << GREEN_COLOR << "yes, I can do it\n" << RESET_COLOR;
            }
            else {
                cout << RED_COLOR << "no, I can't do it\n" << RESET_COLOR;
            }
        }
        else {
            cout << RED_COLOR << fromWhom << " has no connection to " << onWhat << "\n" << RESET_COLOR;
        }
    }

    void commandGrant(const string& Right, const string& toWhom, const string& fromWhom, const string& onWhat) {
        vector<string>right;
        if (Right == "g")
            right = { "g","a" };
        else
            right = { "a" };

        if (database.DFS(toWhom, onWhat, visit ,right)) {
            if (database.DFS(toWhom, fromWhom, visit, {"g","a"})) {
                cout << GREEN_COLOR << "yes, I can do it\n" << RESET_COLOR;
            }
            else {
                cout << RED_COLOR << "no, I can't do it\n" << RESET_COLOR;
            }
        }
        else {
            cout << RED_COLOR << "no, I can't do it\n" << RESET_COLOR;
        }
    }
    //                             Право                К Кому                  Что             Субъект/Объект
    void commandCreate(const string& Right, const string& toWhom, const string& What, const string& type) {
        database.AddVertex(type, toWhom, What, { Right });
        database.ConvertDatabaseToGraph();
        cout << GREEN_COLOR << "The command is executed\n" << RESET_COLOR;
    }

    void commandDelete(const string& fromWhom, const string& toWhom) {
        database.RemoveEdge(fromWhom, toWhom);
        database.ConvertDatabaseToGraph();
    }

public:
    Command(Database& db) : database(db) {}  // Конструктор принимает ссылку на базу данных

    void Parse(const string& command_str) {
        vector<string> cmd_parts;
        istringstream iss(command_str);
        string part;
        while (iss >> part) {
            cmd_parts.push_back(part);
        }
        string cmd = cmd_parts[0];
        if (cmd_parts.size() == 5) {
            if (cmd == "/create") {
                commandCreate(cmd_parts[1], cmd_parts[2], cmd_parts[3], cmd_parts[4]);
            }
            else if (cmd == "/take") {
                commandTake(cmd_parts[1], cmd_parts[2], cmd_parts[3], cmd_parts[4]);
            }
            else if (cmd == "/grant") {
                commandGrant(cmd_parts[1], cmd_parts[2], cmd_parts[3], cmd_parts[4]);
            }
            else {
                cout << "No such command" << endl;
            }
        }
        else if (cmd_parts.size() == 3) {
            if (cmd == "/delete") {
                commandDelete(cmd_parts[1], cmd_parts[2]);
            }
            else {
                cout << "No such command" << endl;
            }
        }
        else {
            cout << "No such command" << endl;
        }
    }
};