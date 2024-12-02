#pragma once
#include <string>

#include "scope.h"
#include "Result.h"
#include "TokenIterator.h"

enum varSetter_Option
{
	endComma,
	endSemiColon,
	endRoundBracket
};

Result<std::string> convertVarSetter(TokenIterator& iterator, MetaData& metaData, const Type& type, FuncInfo& funcInfo, ScopeIterator& scope, const varSetter_Option& option);
