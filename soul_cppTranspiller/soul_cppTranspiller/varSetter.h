#pragma once
#include <string>

#include "Result.h"
#include "TokenIterator.h"

enum varSetter_Option
{
	endComma,
	endSemiColon,
	endRoundBracket
};

Result<std::string> convertVarSetter(TokenIterator& iterator, MetaData& metaData, const Type& type, FuncInfo& funcInfo, const varSetter_Option& option);
