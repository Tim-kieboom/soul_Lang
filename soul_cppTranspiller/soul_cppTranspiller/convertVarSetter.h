#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h" 

enum varSetter_Option
{
	endComma,
	endSemiColon,
	endRoundBracket
};

Result<std::string> convertVarSetter
(
	TokenIterator& iterator, 
	MetaData& metaData, 
	const TypeInfo& type, 
	FuncInfo& funcInfo, 
	ScopeIterator& scope,
	const varSetter_Option& option
);