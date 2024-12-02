#pragma once
#include "scope.h"
#include "metaData.hpp"
#include "TokenIterator.h"

Result<std::string> convertForLoop(TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope);
