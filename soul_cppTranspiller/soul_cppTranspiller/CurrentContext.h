#pragma once
#include "ScopeIterator.h"
#include "ClassInfo.h"
#include "Nullable.h"

struct CurrentContext
{
	ScopeIterator scope;
	Nullable<ClassInfo> inClass;

	CurrentContext(ScopeIterator&& scope)
		: scope(scope)
	{
	}

	CurrentContext(ScopeIterator&& scope, ClassInfo& classInfo)
		: scope(scope), inClass(Nullable<ClassInfo>(classInfo))
	{
	}

	CurrentContext(CurrentContext& other, uint64_t currentScopeIndex)
		: scope(other.scope), inClass(other.inClass)
	{
		scope.currentIndex = currentScopeIndex;
	}
};