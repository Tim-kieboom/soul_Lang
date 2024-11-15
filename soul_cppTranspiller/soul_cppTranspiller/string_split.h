#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

bool initListEquals(const std::initializer_list<char>& splitOnChar, char ch);
bool initListEquals(const std::initializer_list<const char*> splitOnChar, const std::string& ch);

std::vector<std::string> string_split(const std::string& str, char splitOnChar);
std::vector<std::string> string_split(const std::string& str, const std::string& splitOnString);
std::vector<std::string> string_split(const std::string& str, const char* splitOnString);

std::vector<std::string> string_split(const std::string& str, std::initializer_list<char> splitOnChars);
std::vector<std::string> string_split(const std::string& str, std::initializer_list<std::string> splitOnString);
std::vector<std::string> string_split(const std::string& str, std::initializer_list<const char*> splitOnString);

std::vector<std::string> string_splitOn(const std::string& str, char splitOnChar);
std::vector<std::string> string_splitOn(const std::string& str, const std::string& splitOnString);
std::vector<std::string> string_splitOn(const std::string& str, const char* splitOnString);

std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<char> splitOnChars);
std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<std::string> splitOnString);
std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<const char*> splitOnString);

