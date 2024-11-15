#pragma once
#include <vector>
#include <string>

std::vector<uint32_t> string_find(const std::string& str, char findChar, uint64_t offset = 0);
std::vector<uint32_t> string_find(const std::string& str, std::string findStr, uint64_t offset = 0);
std::vector<uint32_t> string_find(const std::string& str, const char* findStr, uint64_t offset = 0);

int64_t string_findFirst(const std::string& str, char findChar, uint64_t offset = 0);
int64_t string_findFirst(const std::string& str, std::string findStr, uint64_t offset = 0);

