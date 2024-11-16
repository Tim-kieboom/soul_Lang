#pragma once
#include <cstdint>
#include <string>

uint32_t string_replace(/*out*/ std::string& str, char replaceChar, char withChar);
uint32_t string_replace(/*out*/ std::string& str, const std::string& replaceStr, const std::string& withStr);
uint32_t string_replace(/*out*/ std::string& str, const char* replaceStr, const char* withStr);
uint32_t string_replace(/*out*/ std::string& str, const std::string& replaceStr, const char* withStr);
uint32_t string_replace(/*out*/ std::string& str, const char* replaceStr, const std::string& withStr);

uint32_t string_replace(/*out*/ std::string& str, char replaceChar, char withChar, uint64_t countLimit);
uint32_t string_replace(/*out*/ std::string& str, const std::string& replaceStr, const std::string& withStr, uint64_t countLimit);
uint32_t string_replace(/*out*/ std::string& str, const char* replaceStr, const char* withStr, uint64_t countLimit);
uint32_t string_replace(/*out*/ std::string& str, const std::string& replaceStr, const char* withStr, uint64_t countLimit);
uint32_t string_replace(/*out*/ std::string& str, const char* replaceStr, const std::string& withStr, uint64_t countLimit);

bool string_replaceAt(std::string& str, int64_t begin, int64_t end, const char* replaceStr);
bool string_replaceAt(std::string& str, int64_t begin, int64_t end, const std::string& replaceStr);
bool string_replaceAt(std::string& str, int64_t index, const char* replaceStr);
bool string_replaceAt(std::string& str, int64_t index, const std::string& replaceStr);

bool string_replaceInSpan(std::string& str, const char* replaceStr, const char* withString, uint32_t begin, uint32_t end);

