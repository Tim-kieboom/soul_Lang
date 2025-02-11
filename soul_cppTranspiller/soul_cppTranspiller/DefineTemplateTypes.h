#pragma once
#include <map>

#include "TemplateType.h"
#include "SuperExpression.h"

class DefineTemplateTypes : public SuperExpression
{
public:
	std::map<std::string, TemplateType> templateTypes;

	DefineTemplateTypes() { }
	DefineTemplateTypes(std::map<std::string, TemplateType>& templateTypes)
		: templateTypes(std::move(templateTypes)) 
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Templates::<";
		for (auto& type : templateTypes)
			ss << type.first << ", ";
		ss << ">";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::DefineTemplateTypes;
	}
};
