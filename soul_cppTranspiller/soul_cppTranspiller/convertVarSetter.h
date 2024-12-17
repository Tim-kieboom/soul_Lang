#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h" 
#include "sstream"

enum varSetter_Option
{
	endComma,
	endSemiColon,
	endRoundBracket,
	endComma_or_RoundBrasket
};

Result<std::string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	const varSetter_Option& option,
	bool addEndl = true
);

Result<std::string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	const varSetter_Option& option,
	int64_t& openBracketStack,
	bool addEndl
);