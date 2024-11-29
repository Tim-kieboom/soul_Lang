#pragma once
#include <string>
#include <vector>

#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFuncCall(TokenIterator& iterator, MetaData& metaData, Nesting& currentScope, FuncInfo& callFunc, FuncInfo& funcInfo);
