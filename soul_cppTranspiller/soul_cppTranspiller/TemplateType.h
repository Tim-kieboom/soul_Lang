#pragma once
#include "Nullable.h"
#include "BinairyExpression.h"

struct TemplateType 
{
	std::string typeName = "";
	Nullable<std::shared_ptr<BinaryExpression>> condition;

	TemplateType() = default;
	TemplateType(std::string& typeName) 
		: typeName(typeName), condition(/*null*/)
	{
	}

	TemplateType(std::string& typeName, BinaryExpression condition)
		: typeName(typeName), condition(std::make_shared<BinaryExpression>(condition))
	{
	}

	TemplateType(std::string& typeName, std::shared_ptr<BinaryExpression> condition)
		: typeName(typeName), condition(condition)
	{
	}
};