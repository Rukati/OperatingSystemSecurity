#pragma once
#include <string>

std::string Hash(std::string ClientPass, std::string ClientSalt);
std::string SQL(char login[256], std::string query);
void SQL_insert(char login[256], char solt[257], char hash[256]);