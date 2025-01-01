#pragma once
#include <string>
#include <initializer_list>

bool string_contains(const std::string& str, const std::string& find);
bool string_contains(const std::string& str, const char* find);
bool string_contains(const std::string& str, char find);
bool string_contains(const std::string& str, std::initializer_list<char>& find);
bool string_containsMultiple(const std::string& str, std::initializer_list<const char*> find);

uint32_t string_count(const std::string& str, char findChar);
uint32_t string_count(const std::string& str, const char* findStr);
uint32_t string_count(const std::string& str, std::string findStr);