#pragma once
#include "ScopeIterator.h"

struct CurrentContext
{
	ScopeIterator scope;

	CurrentContext(ScopeIterator&& scope)
		: scope(scope)
	{
	}
};