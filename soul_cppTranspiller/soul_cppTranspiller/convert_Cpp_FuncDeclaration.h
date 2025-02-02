#pragma once
#include <sstream>

#include "MetaData.h"

Result<std::string> convert_Cpp_FuncDeclaration(FuncDeclaration& funcInfo, MetaData& metaData);