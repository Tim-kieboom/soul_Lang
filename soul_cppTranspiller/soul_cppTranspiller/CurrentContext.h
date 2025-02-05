#pragma once
#include "ScopeIterator.h"
#include "Nullable.h"

struct CurrentContext
{
	ScopeIterator scope;
	Nullable<ClassInfo> classInfo;

	CurrentContext(ScopeIterator&& scope)
		: scope(scope)
	{
	}

	CurrentContext(ScopeIterator&& scope, ClassInfo& classInfo)
		: scope(scope), classInfo(Nullable<ClassInfo>(classInfo))
	{
	}
};