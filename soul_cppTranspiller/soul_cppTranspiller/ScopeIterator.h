#pragma once
#include "Result.h"
#include "Nesting.h"

struct ScopeIterator
{
	std::vector<Nesting>& scope;
	uint64_t currentIndex;

	ScopeIterator(std::vector<Nesting>& scope)
		: scope(scope), currentIndex(0)
	{
	}

	ScopeIterator(std::vector<Nesting>& scope, uint64_t currentNestingIndex)
		: scope(scope), currentIndex(currentNestingIndex)
	{
	}

	Result<VarInfo*> tryGetVariable_fromCurrent(const std::string& name, std::vector<VarInfo>& globalScope, uint64_t currentLine)
	{	
		if(scope.size() == 0)
			return ErrorInfo("scope is empty", currentLine);

		Result<VarInfo*> result = getCurrentNesting().tryGetVariable(name, this->scope, globalScope);
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