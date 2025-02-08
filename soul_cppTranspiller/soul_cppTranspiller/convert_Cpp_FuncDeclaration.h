#pragma once
#include <sstream>
#include <unordered_set>
#include "MetaData.h"

Result<std::string> convert_Cpp_FuncDeclaration(FuncDeclaration& funcInfo, MetaData& metaData, std::unordered_set<std::string>& templateTypes, bool* isMethode = nullptr);