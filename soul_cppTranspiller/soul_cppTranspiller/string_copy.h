#pragma once
#include <string>
#include <vector>

const char* string_copyTo_c_str(const std::string& str);

std::string concatToString(const std::vector<std::string>& vect);
std::string concatToString(const std::vector<std::string>& vect, const std::string& addBetweenStrs);
