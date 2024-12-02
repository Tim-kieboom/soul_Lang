#pragma once
#include <string>
#include <vector>

#include "scope.h"
#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFuncCall(TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, FuncInfo& callFunc, FuncInfo& funcInfo);
