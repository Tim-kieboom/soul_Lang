#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"

Result<std::string> convertFunctionCall
(
	TokenIterator& iterator, 
	MetaData& metaData, 
	ScopeIterator& scope, 
	std::string& callFuncName,
	FuncInfo& funcInfo,
	std::string* className
);
