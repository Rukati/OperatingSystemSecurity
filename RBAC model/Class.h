#include "ColorText.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <E:\Library\rapidjson-master\include\rapidjson/document.h>
#include <E:\Library\rapidjson-master\include\rapidjson/writer.h>
#include <E:\Library\rapidjson-master\include\rapidjson/stringbuffer.h>
#include <E:\Library\rapidjson-master\include\rapidjson/ostreamwrapper.h> 

using namespace rapidjson;

class Access {
private:
    Document doc;

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> roles;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> objects;
    std::vector<std::string> subjects;
    std::unordered_map<std::string, bool> possible_right;

    bool chek(std::string object) {
        return objects.count(object) > 0;
    }

public:
    //������������� ������
    Access() {
        // ��������� JSON-���� ��� ������
        std::ifstream file("roles.json");
        if (!file.is_open()) {
            std::cerr << "������ �������� �����!" << std::endl;
            return;
        }

        // ������ ���������� ����� � ������
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonString = buffer.str();

        // ������ JSON-������ � ������ Document
        if (doc.Parse(jsonString.c_str()).HasParseError()) {
            std::cerr << "������ �������� JSON!" << std::endl;
            return;
        }

        // �������������� ����
        if (doc.HasMember("roles") && doc["roles"].IsObject()) {
            const Value& rolesValue = doc["roles"];
            for (Value::ConstMemberIterator roleIt = rolesValue.MemberBegin(); roleIt != rolesValue.MemberEnd(); ++roleIt) {
                const std::string& roleName = roleIt->name.GetString();
                const Value& roleValue = roleIt->value;

                if (roleValue.HasMember("subjects") && roleValue["subjects"].IsArray()) {
                    const Value& subjectsArray = roleValue["subjects"];
                    std::vector<std::string> roleSubjects;
                    for (SizeType i = 0; i < subjectsArray.Size(); ++i) {
                        if (subjectsArray[i].IsString()) {
                            roleSubjects.push_back(subjectsArray[i].GetString());
                        }
                    }
                    roles[roleName]["subjects"] = roleSubjects;
                }

                if (roleValue.HasMember("objects") && roleValue["objects"].IsObject()) {
                    const Value& objectsValue = roleValue["objects"];
                    for (Value::ConstMemberIterator objectIt = objectsValue.MemberBegin(); objectIt != objectsValue.MemberEnd(); ++objectIt) {
                        const std::string& objectName = objectIt->name.GetString();
                        const Value& objectValue = objectIt->value;

                        if (objectValue.IsArray()) {
                            std::vector<std::string> permissions;
                            for (SizeType i = 0; i < objectValue.Size(); ++i) {
                                if (objectValue[i].IsString()) {
                                    permissions.push_back(objectValue[i].GetString());
                                }
                            }
                            objects[objectName][roleName] = permissions;
                        }
                    }
                }
            }
        }
        // ������������� ����
        if (doc.HasMember("permissions") && doc["permissions"].IsObject()) {
            const Value& permissionsValue = doc["permissions"];
            for (Value::ConstMemberIterator permIt = permissionsValue.MemberBegin(); permIt != permissionsValue.MemberEnd(); ++permIt) {
                const std::string& permission = permIt->name.GetString();
                possible_right[permission] = false;
            }
        }

    }

    void PrintData() const {
        for (const auto& role : roles) {
            std::cout << "Role: " << role.first << std::endl;

            const auto& roleData = role.second;

            std::cout << "Subjects: ";
            const auto& subjects = roleData.at("subjects");
            for (const auto& subject : subjects) {
                std::cout << subject << ", ";
            }
            std::cout << std::endl;

            std::cout << "Objects: " << std::endl;
            for (const auto& object : objects) {
                std::cout << object.first << " [ ";

                const auto& permissions = object.second.find(role.first);
                if (permissions != object.second.end()) {
                    for (const auto& permission : permissions->second) {
                        std::cout << permission << ", ";
                    }
                }

                std::cout << " ] " << std::endl;
            }

            std::cout << std::endl;
        }
    }

    void createNewRole() {
        std::string roleName;
        std::cout << "������� ��� ����: ";
        std::getline(std::cin, roleName);

        if (roles.find(roleName) != roles.end()) {
            std::cout << BRIGHT_RED_COLOR << "����� ���� ��� ����������." << RESET_COLOR << std::endl;
            return;
        }

        roles[roleName]["subjects"] = std::vector<std::string>();

        std::vector<std::string> parentRoles;
        std::string parent;

        int i = 1;
        do
        {
            std::cout << "��������" << i <<": ";
            std::getline(std::cin, parent);

            // �������� �� ������������� ����
            if (roles.count(parent) > 0) {
                parentRoles.push_back(parent);
                i++;
                break;
            }
            else {
                std::cout << BRIGHT_RED_COLOR << "���� �� ����������!" << RESET_COLOR << std::endl;
            }

        } while (true);

        while (!parent.empty()) {

            std::cout << "��������" << i << ": ";
            std::getline(std::cin, parent);

            if (roles.count(parent) > 0) {
                parentRoles.push_back(parent);
                i++;
            }
            else {
                if (!parent.empty())
                    std::cout << BRIGHT_RED_COLOR << "���� �� ����������!" << RESET_COLOR << std::endl;
            }
        }

        std::vector<std::string> possibleSubjects;
        for (const auto& parentRole : parentRoles) {
            const auto& parentSubjects = roles[parentRole]["subjects"];
            if (possibleSubjects.empty()) {
                possibleSubjects = parentSubjects;
            }
            else {
                std::vector<std::string> intersection;
                for (const auto& subject : parentSubjects) {
                    if (std::find(possibleSubjects.begin(), possibleSubjects.end(), subject) != possibleSubjects.end()) {
                        intersection.push_back(subject);
                    }
                }
                possibleSubjects = intersection;
            }
        }

        if (possibleSubjects.empty()) {
            std::cout << BRIGHT_RED_COLOR << "��� ������� ���������." << RESET_COLOR << std::endl;
            return;
        }
        else {
            std::cout << "����������� �������� � �����: ";
            for (const auto& subject : possibleSubjects) {
                std::cout << subject << " ";
            }
            std::cout << std::endl;
        }

        std::vector<std::string> subjectRoles;
        std::string subject;
        std::cout << "subj: ";
        std::getline(std::cin, subject);
        while (!subject.empty()) {
            if (std::find(possibleSubjects.begin(), possibleSubjects.end(), subject) != possibleSubjects.end()) {
                subjectRoles.push_back(subject);
                if (std::find(subjects.begin(), subjects.end(), subject) == subjects.end()) {
                    subjects.push_back(subject);
                }
                std::cout << "subj: ";
                std::getline(std::cin, subject);
            }
            else {
                std::cout << BRIGHT_RED_COLOR << "������ �������� �������: " << subject << RESET_COLOR << std::endl;
                return;
            }
        }

        roles[roleName]["subjects"] = subjectRoles;

        std::cout << CYAN_COLOR << "Objects: " << RESET_COLOR;
        for (const auto& obj : objects)
        {
            std::cout << obj.first <<",";
        }
        std::cout << std::endl;

        std::string object;
        std::cout << "object: ";
        std::getline(std::cin, object);

        
        while (!object.empty()) {
            
            if (chek(object))
            {
                std::unordered_map<std::string, std::vector<std::string>> possibleRights;
                for (const auto& parentRole : parentRoles) {
                    if (objects.find(object) != objects.end() && objects[object].find(parentRole) != objects[object].end()) {
                        possibleRights[parentRole] = objects[object][parentRole];
                    }
                    else {
                        possibleRights[parentRole] = {};
                    }
                }

                std::vector<std::string> permissions;
                std::cout << BRIGHT_CYAN_COLOR << "Possible rights: " << CYAN_COLOR;
                for (const auto& entry : possibleRights) {
                    const std::vector<std::string>& rights = entry.second;
                    for (const auto& right : rights) {
                        if (!possible_right[right]) {
                            permissions.push_back(right);
                            std::cout << right << ", ";
                            possible_right[right] = true;
                        }
                    }
                }
                std::cout << std::endl;

                std::vector<std::string> availableRights;
                if (!possibleRights.empty()) {
                    for (const auto& entry : possible_right) {
                        if (!entry.second)
                            availableRights.push_back(entry.first);
                    }


                    std::cout << BRIGHT_CYAN_COLOR << "Available rights: " << CYAN_COLOR;
                    for (const auto& right : availableRights) {
                        std::cout << right << ", ";
                    }
                    std::cout << RESET_COLOR << std::endl;
                }

                std::string permission;
                if (availableRights.size() > 0) {
                    do
                    {
                        std::cout << MAGENTA_COLOR << "Permission: " << RESET_COLOR;
                        std::getline(std::cin, permission);
                        if (!permission.empty())
                            if (possible_right.count(permission) > 0)
                                break;
                            else
                                std::cout << "����� ����� �� ��������" << std::endl;
                    } while (true && !permission.empty());

                    while (!permission.empty() && availableRights.size() > 0) {

                        if (possible_right.count(permission) > 0)
                        {
                            if (!possible_right[permission]) {
                                permissions.push_back(permission);
                                possible_right[permission] = true;
                                std::cout << BRIGHT_GREEN_COLOR << "�������" << std::endl;
                            }
                            else
                                std::cout << BRIGHT_RED_COLOR << "����� ��� ���������" << RESET_COLOR << std::endl;

                        }
                        else
                            if(!permission.empty())
                                std::cout << "����� ����� �� ��������" << std::endl;
                        
                        std::cout << MAGENTA_COLOR << "Permission: " << RESET_COLOR;
                        std::getline(std::cin, permission);

                    }
                }
                objects[object][roleName] = permissions;

                permissions.clear();
                for (auto& right : possible_right)
                    right.second = false;
            }
            else
            {
                std::cout << BRIGHT_RED_COLOR << "������ object �� ����������.\n" << RESET_COLOR;
            }
            std::cout << "object: ";
            std::getline(std::cin, object);
        }


        std::cout << "���� �������." << std::endl;
        SaveData();
    }

    void SaveData() const {
        // ������� ������ Writer ��� ������ JSON
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);

        // ��������� ������ ������, ���������� � ���������
        writer.StartObject();

        // ���������� ����
        writer.Key("roles");
        writer.StartObject();
        for (const auto& role : roles) {
            writer.Key(role.first.c_str());
            writer.StartObject();

            // ���������� �������� ����
            writer.Key("subjects");
            writer.StartArray();
            const auto& subjects = role.second.at("subjects");
            for (const auto& subject : subjects) {
                writer.String(subject.c_str());
            }
            writer.EndArray();

            // ���������� ������� � ���������� ��� ����
            writer.Key("objects");
            writer.StartObject();
            for (const auto& object : objects) {
                const auto& permissions = object.second.find(role.first);
                if (permissions != object.second.end()) {
                    writer.Key(object.first.c_str());
                    writer.StartArray();
                    for (const auto& permission : permissions->second) {
                        writer.String(permission.c_str());
                    }
                    writer.EndArray();
                }
            }
            writer.EndObject();

            writer.EndObject();
        }
        writer.EndObject();

        // ���������� possible_right
        writer.Key("permissions");
        writer.StartObject();
        for (const auto& permission : possible_right) {
            writer.Key(permission.first.c_str());
            writer.StartObject();
            writer.EndObject();
        }
        writer.EndObject();

        writer.EndObject();

        // �������� ������ JSON
        std::string jsonString = buffer.GetString();

        // ��������� JSON-���� ��� ������
        std::ofstream file("roles.json");
        if (!file.is_open()) {
            std::cerr << "������ �������� �����!" << std::endl;
            return;
        }

        // ���������� JSON-������ � ����
        file << jsonString;

        std::cout << "������ ��������� � ����." << std::endl;
    }

};
