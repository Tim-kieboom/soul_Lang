#pragma once
#include <string>

#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFuncCall(TokenIterator& iterator, MetaData& metaData, FuncInfo callFunc, FuncInfo funcInfo);
