#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"

Result<std::string> convertIndexer(TokenIterator& iterator, FuncInfo& funcInfo, ScopeIterator& scope, MetaData& metaData);
