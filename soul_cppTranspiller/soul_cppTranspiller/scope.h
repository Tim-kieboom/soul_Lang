#pragma once
#include <string>

#include "MetaData.hpp"

struct ScopeIterator
{
	std::vector<Nesting>& scope;
	uint64_t currentNestingIndex = 0;

	ScopeIterator(std::vector<Nesting>& scope)
		: scope(scope)
	{
	}

	ScopeIterator(std::vector<Nesting>& scope, uint64_t currentNestingIndex)
		: scope(scope), currentNestingIndex(currentNestingIndex)
	{
	}

	Nesting& getCurrentNesting()
	{
		if (currentNestingIndex+1 > scope.size())
			throw std::exception("currentNestingIndex is bigger then scope");

		return scope.at(currentNestingIndex);
	}
};