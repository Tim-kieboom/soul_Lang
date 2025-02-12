#pragma once
#include <sstream>
#include <unordered_map>
#include "MetaData.h"

Result<std::string> convert_Cpp_FuncDeclaration(FuncDeclaration& funcInfo, MetaData& metaData, std::map<std::string, TemplateType>& templateTypes, bool* isMethode = nullptr);