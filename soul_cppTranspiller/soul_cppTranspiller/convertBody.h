#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"	

Result<std::string> convertBody(TokenIterator& iterator, /*out*/ FuncInfo& funcInfo, MetaData& metaData, ScopeIterator& scope, uint32_t depth, std::string* className = nullptr);
Result<std::string> convertBody_inClass(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData, ClassInfo& classInfo, ScopeIterator& scope);