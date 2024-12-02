#pragma once
#include "scope.h"
#include "Result.h"
#include "metaData.hpp"
#include "TokenIterator.h"

Result<std::string> convertVarInit(Type type, bool isMutable, TokenIterator& iterator, MetaData& metaData, FuncInfo& callFunc, FuncInfo& funcInfo, ScopeIterator& scope);