#pragma once
#include <unordered_set>
#include "ScopeIterator.h"
#include "ClassInfo.h"
#include "Nullable.h"

struct CurrentContext
{
	enum class FuncRuleSet { invalid, default_, Functional };

	ScopeIterator scope;
	Nullable<ClassInfo> inClass;
	std::unordered_set<std::string> currentTemplateTypes;
	FuncRuleSet functionRuleSet = FuncRuleSet::default_;

	static FuncRuleSet getFuncRuleSet(std::string& token)
	{
		if (token == "func")
			return FuncRuleSet::default_;
		else if (token == "Functional")
			return FuncRuleSet::Functional;

		return FuncRuleSet::invalid;
	}

	CurrentContext(ScopeIterator&& scope)
		: scope(scope)
	{
	}

	CurrentContext(ScopeIterator&& scope, ClassInfo& classInfo)
		: scope(scope), inClass(Nullable<ClassInfo>(classInfo))
	{
	}

	CurrentContext(CurrentContext& other, uint64_t currentScopeIndex)
		: scope(other.scope), inClass(other.inClass), functionRuleSet(other.functionRuleSet), currentTemplateTypes(other.currentTemplateTypes)
	{
		scope.currentIndex = currentScopeIndex;
	}
};