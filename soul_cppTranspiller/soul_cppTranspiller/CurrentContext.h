#pragma once
#include <map>
#include <unordered_map>
#include "Nullable.h"
#include "ClassInfo.h"
#include "ScopeIterator.h"
#include "TemplateType.h"

struct CurrentContext
{
	enum class FuncRuleSet { invalid, default_, Functional };
	struct MemberInfo 
	{
		std::string type = "<invalid>";
		bool isAllowedToUsePrivate = false;

		MemberInfo() = default;
		MemberInfo(std::string type, bool isAllowedToUsePrivate)
			: type(type), isAllowedToUsePrivate(isAllowedToUsePrivate)
		{
		}
	};

	ScopeIterator scope;
	Nullable<ClassInfo> inClass;
	Nullable<MemberInfo> parentOfCurrentMember;
	std::map<std::string, TemplateType> currentTemplateTypes;
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