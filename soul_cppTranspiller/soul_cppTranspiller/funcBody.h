#pragma once
#include <string>
#include <vector>

#include "scope.h"
#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFunctionBody(TokenIterator& iterator, /*out*/ FuncInfo& funcInfo, MetaData& metaData, ScopeIterator& scope);

