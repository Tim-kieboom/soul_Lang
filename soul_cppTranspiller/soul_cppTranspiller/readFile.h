#pragma once
#include <cstdint>
#include <string>

std::string readFile(const char* path, int64_t& lineCount);
std::string readFile_andStoreLine(const char* path, int64_t& lineCount);


