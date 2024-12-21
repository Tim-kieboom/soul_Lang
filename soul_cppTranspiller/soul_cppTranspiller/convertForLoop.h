#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"

Result<std::string> convertForLoop(TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope);
