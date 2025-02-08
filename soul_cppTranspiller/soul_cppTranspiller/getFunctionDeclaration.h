#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "TemplateTypes.h"
#include "FuncDeclaration.h"

struct FuncDeclaration_Result
{
	FuncDeclaration funcInfo;
	std::shared_ptr<TemplateTypes> templateTypes;

	FuncDeclaration_Result() { }
	FuncDeclaration_Result(FuncDeclaration& funcInfo, std::shared_ptr<TemplateTypes>& templateTypes)
		: funcInfo(funcInfo), templateTypes(templateTypes) 
	{
	}
};

Result<FuncDeclaration_Result> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData, bool isForwardDeclared, CurrentContext& context, ClassInfo* currentClass = nullptr);
