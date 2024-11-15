#pragma once
#include <string>

bool string_removeSpan(/*out*/ std::string& str, int64_t begin, int64_t end);
bool string_removeAt(/*out*/ std::string& str, int64_t index);
uint64_t string_remove(/*out*/ std::string& str, char ch);
uint64_t string_remove(/*out*/ std::string& str, std::string ch);
uint64_t string_remove(/*out*/ std::string& str, const char* ch);

