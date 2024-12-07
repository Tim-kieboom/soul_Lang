#pragma once
#include <string>

#include "Result.h"
#include "Nesting.h"

struct ScopeIterator
{
	std::vector<Nesting>& scope;
	uint64_t currentIndex = 0;

	ScopeIterator(std::vector<Nesting>& scope)
		: scope(scope)
	{
	}

	ScopeIterator(std::vector<Nesting>& scope, uint64_t currentNestingIndex)
		: scope(scope), currentIndex(currentNestingIndex)
	{
	}

	Result<VarInfo> tryGetVariable_fromCurrent(const std::string& name, const std::vector<VarInfo>& globalScope, uint64_t currentLine)
	{
		Result<VarInfo> result = getCurrentNesting().tryGetVariable(name, this->scope, globalScope);
		if (result.hasError)
			return ErrorInfo(result.error.message, currentLine);

		return result;
	}

	Nesting& getCurrentNesting()
	{
		if (currentIndex + 1 > scope.size())
			throw std::exception("currentNestingIndex is bigger then scope");

		return scope.at(currentIndex);
	}
};