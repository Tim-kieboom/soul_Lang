#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"

Result<std::string> convertVarInit(TypeInfo& type, TokenIterator& iterator, MetaData& metaData, FuncInfo& callFunc, FuncInfo& funcInfo, ScopeIterator& scope, std::string* className);
