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

typedef Result<bool>(*EndVarSetter)
(
	TokenIterator& iterator,
	MetaData& metaData,
	std::stringstream& ss,
	uint32_t openBracketStack,
	const varSetter_Option& option,
	bool addEndl
);

Result<std::string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	EndVarSetter endVarFunc,
	const varSetter_Option& option,
	bool addEndl
);

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