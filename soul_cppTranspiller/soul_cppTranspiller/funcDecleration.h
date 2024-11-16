#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFuncDeclaration(/*out*/TokenIterator& iterator, /*out*/MetaData& metaData, /*out*/FuncInfo& funcInfo);


