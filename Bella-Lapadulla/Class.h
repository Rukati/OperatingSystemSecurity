#pragma once

#include "ColorsText.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>

using namespace rapidjson;

class Access {
private:
    Document db;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> access_matrix;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> subjects;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> objects;

    void remove(const std::string& subj) {
        std::vector<std::string> objs;
        for (const auto& entry : access_matrix[subj]) {
            objs.push_back(entry.first);
        }

        for (const std::string& obj : objs) {
            std::vector<std::string>& rights = access_matrix[subj][obj];
            int cur_mand_subj = subjects[subj]["current_mandate"];
            int mand_obj = objects[obj]["mandate"];

            if (std::find(rights.begin(), rights.end(), "read") != rights.end() && cur_mand_subj < mand_obj) {
                rights.erase(std::remove(rights.begin(), rights.end(), "read"), rights.end());
            }
            if (std::find(rights.begin(), rights.end(), "append") != rights.end() && cur_mand_subj < mand_obj) {
                rights.erase(std::remove(rights.begin(), rights.end(), "append"), rights.end());
            }
            if (std::find(rights.begin(), rights.end(), "write") != rights.end() && cur_mand_subj != mand_obj) {
                rights.erase(std::remove(rights.begin(), rights.end(), "write"), rights.end());
            }
        }
    }

    bool GettingAccess(const std::string& subj, const std::string& obj, const std::string& right) {
        int mand_subj = subjects[subj]["mandate"];
        int cur_mand_subj = subjects[subj]["current_mandate"];
        int mand_obj = objects[obj]["mandate"];

        //-------------------SS-------------------
        if (right == "execute" || right == "append") {
            std::cout << GREEN_COLOR << "SS_property - Success" << RESET_COLOR << std::endl;
        }
        else if (right == "read" || right == "write") {
            if (mand_subj >= mand_obj) {
                std::cout << GREEN_COLOR << "SS_property - Success" << RESET_COLOR << std::endl;
            }
            else {
                std::cout << BRIGHT_RED_COLOR << "SS_property - Fail" << RESET_COLOR << std::endl;
                return false;
            }
        }

        //-------------------STAR-------------------
        if (right == "execute") {
            access_matrix[subj][obj].push_back(right);
            std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
            return true;
        }
        else if (right == "append") {
            if (mand_obj >= cur_mand_subj) {
                access_matrix[subj][obj].push_back(right);
                std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
                return true;
            }
            cur_mand_subj = mand_obj;
            subjects[subj]["current_mandate"] = mand_obj;
            access_matrix[subj][obj].push_back(right);

            remove(subj);

            std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
            return true;
        }
        else if (right == "read") {
            if (cur_mand_subj >= mand_obj) {
                access_matrix[subj][obj].push_back(right);
                std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
                return true;
            }
            else if (mand_subj >= mand_obj) {
                cur_mand_subj = mand_obj;
                subjects[subj]["current_mandate"] = mand_obj;
                access_matrix[subj][obj].push_back(right);

                remove(subj);

                std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
                return true;
            }
            else {
                std::cout << BRIGHT_RED_COLOR << "STAR_property - Fail" << RESET_COLOR << std::endl;
                return false;
            }
        }
        else if (right == "write") {
            if (cur_mand_subj == mand_obj) {
                access_matrix[subj][obj].push_back(right);
                std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
                return true;
            }
            if (mand_subj >= mand_obj) {
                subjects[subj]["current_mandate"] = mand_obj; 
                cur_mand_subj = mand_obj;
                access_matrix[subj][obj].push_back(right);

                remove(subj);

                std::cout << GREEN_COLOR << "STAR_property - Success" << RESET_COLOR << std::endl;
                return true;
            }
        }
        else
            std::cout << BRIGHT_RED_COLOR << "Fail" << RESET_COLOR << std::endl;
        return false;
    }

public:

    Access() {
        // Èíèöèàëèçàöèÿ ïóñòîãî JSON-îáúåêòà
        db.SetObject();

        std::ifstream file("access_mandate.json");
        if (file.is_open()) {
            std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            if (!db.Parse(jsonString.c_str()).HasParseError()) {
                // Çàãðóçêà äàííûõ èç JSON-îáúåêòà
                if (db.HasMember("matrix") && db["matrix"].IsObject()) {
                    const Value& matrixObj = db["matrix"];
                    for (Value::ConstMemberIterator subjIt = matrixObj.MemberBegin(); subjIt != matrixObj.MemberEnd(); ++subjIt) {
                        const std::string& subj = subjIt->name.GetString();
                        if (subjIt->value.IsObject()) {
                            const Value& subjObj = subjIt->value;
                            for (Value::ConstMemberIterator objIt = subjObj.MemberBegin(); objIt != subjObj.MemberEnd(); ++objIt) {
                                const std::string& obj = objIt->name.GetString();
                                if (objIt->value.IsArray()) {
                                    const Value& rightsArr = objIt->value;
                                    std::vector<std::string> rights;
                                    for (SizeType i = 0; i < rightsArr.Size(); ++i) {
                                        if (rightsArr[i].IsString()) {
                                            const std::string& right = rightsArr[i].GetString();
                                            rights.push_back(right);
                                        }
                                    }
                                    access_matrix[subj][obj] = rights;
                                }
                            }
                        }
                    }
                }

                if (db.HasMember("subjects") && db["subjects"].IsObject()) {
                    const Value& subjectsObj = db["subjects"];
                    for (Value::ConstMemberIterator subjIt = subjectsObj.MemberBegin(); subjIt != subjectsObj.MemberEnd(); ++subjIt) {
                        const std::string& subj = subjIt->name.GetString();
                        if (subjIt->value.IsObject()) {
                            const Value& subjObj = subjIt->value;
                            for (Value::ConstMemberIterator mandIt = subjObj.MemberBegin(); mandIt != subjObj.MemberEnd(); ++mandIt) {
                                const std::string& mand = mandIt->name.GetString();
                                if (mandIt->value.IsInt()) {
                                    int mandate = mandIt->value.GetInt();
                                    subjects[subj][mand] = mandate;
                                }
                            }
                        }
                    }
                }

                if (db.HasMember("objects") && db["objects"].IsObject()) {
                    const Value& objectsObj = db["objects"];
                    for (Value::ConstMemberIterator objIt = objectsObj.MemberBegin(); objIt != objectsObj.MemberEnd(); ++objIt) {
                        const std::string& obj = objIt->name.GetString();
                        if (objIt->value.IsObject()) {
                            const Value& objObj = objIt->value;
                            for (Value::ConstMemberIterator mandIt = objObj.MemberBegin(); mandIt != objObj.MemberEnd(); ++mandIt) {
                                const std::string& mand = mandIt->name.GetString();
                                if (mandIt->value.IsInt()) {
                                    int mandate = mandIt->value.GetInt();
                                    objects[obj][mand] = mandate;
                                }
                            }
                        }
                    }
                }

                //std::cout << "\033[1;32mÔàéë óñïåøíî çàãðóæåí.\033[0m" << std::endl;
            }
            else {
                std::cout << BRIGHT_RED_COLOR << "Îøèáêà ïðè ðàçáîðå ôàéëà JSON." << RESET_COLOR << std::endl;
            }
        }
    }

    void SaveToFile() {
        // Çàïîëíåíèå äàííûõ â JSON-îáúåêòå
        Value matrixObj(kObjectType);
        for (const auto& subjEntry : access_matrix) {
            const std::string& subj = subjEntry.first;
            const auto& objMap = subjEntry.second;
            Value subjObj(kObjectType);
            for (const auto& objEntry : objMap) {
                const std::string& obj = objEntry.first;
                const auto& rights = objEntry.second;
                Value rightsArr(kArrayType);
                for (const std::string& right : rights) {
                    Value rightValue(right.c_str(), db.GetAllocator());
                    rightsArr.PushBack(rightValue, db.GetAllocator());
                }
                subjObj.AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), rightsArr.Move(), db.GetAllocator());
            }
            matrixObj.AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), subjObj.Move(), db.GetAllocator());
        }

        Value subjectsObj(kObjectType);
        for (const auto& subjEntry : subjects) {
            const std::string& subj = subjEntry.first;
            const auto& mandMap = subjEntry.second;
            Value subjObj(kObjectType);
            for (const auto& mandEntry : mandMap) {
                const std::string& mand = mandEntry.first;
                int mandate = mandEntry.second;
                subjObj.AddMember(Value(mand.c_str(), db.GetAllocator()).Move(), mandate, db.GetAllocator());
            }
            subjectsObj.AddMember(Value(subj.c_str(), db.GetAllocator()).Move(), subjObj.Move(), db.GetAllocator());
        }

        Value objectsObj(kObjectType);
        for (const auto& objEntry : objects) {
            const std::string& obj = objEntry.first;
            const auto& mandMap = objEntry.second;
            Value objObj(kObjectType);
            for (const auto& mandEntry : mandMap) {
                const std::string& mand = mandEntry.first;
                int mandate = mandEntry.second;
                objObj.AddMember(Value(mand.c_str(), db.GetAllocator()).Move(), mandate, db.GetAllocator());
            }
            objectsObj.AddMember(Value(obj.c_str(), db.GetAllocator()).Move(), objObj.Move(), db.GetAllocator());
        }

        db.RemoveAllMembers();
        db.AddMember("matrix", matrixObj.Move(), db.GetAllocator());
        db.AddMember("subjects", subjectsObj.Move(), db.GetAllocator());
        db.AddMember("objects", objectsObj.Move(), db.GetAllocator());

        // Ñîõðàíåíèå äàííûõ â ôàéë
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        db.Accept(writer);

        std::ofstream file("access_mandate.json");
        if (file.is_open()) {
            file << buffer.GetString();
            file.close();
            //std::cout << "\033[1;32mÄàííûå óñïåøíî ñîõðàíåíû â ôàéë.\033[0m" << std::endl;
        }
        else {
            std::cout << BRIGHT_RED_COLOR << "Îøèáêà ïðè ñîõðàíåíèè äàííûõ â ôàéë." << RESET_COLOR << std::endl;
        }
    }

    void PrintContents() {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        db.Accept(writer);

        std::cout << "Ñîäåðæèìîå:";
        std::cout << buffer.GetString() << std::endl;
    }

    void AddRight(const std::string& subj, const std::string& obj, const std::string& right) {
        if (GettingAccess(subj, obj, right)) {
            std::vector<std::string>& rights = access_matrix[subj][obj];
            if (std::find(rights.begin(), rights.end(), right) == rights.end()) {
                rights.push_back(right);
            }
        }
    }

    bool ChekSubj(std::string subj)
    {
        if (!db["subjects"].HasMember(subj.c_str())) {
            std::cout << "\033[1;31m>>> Ñóáúåêò íå ñóùåñòâóåò.\033[0m" << std::endl;
            return false;
        }
        else
            return true;
    }

    bool ChekObj(std::string obj)
    {
        if (!db["objects"].HasMember(obj.c_str())) {
            std::cout << "\033[1;31m>>> Îáúåêò íå ñóùåñòâóåò.\033[0m" << std::endl;
            return false;
        }
        else
            return true;
    }

};
