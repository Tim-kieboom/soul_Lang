#pragma once
#include "SuperExpression.h"

class TemplateTypes: public SuperExpression
{
public:
	std::unordered_set<std::string> templateTypes;

	TemplateTypes() { }
	TemplateTypes(std::unordered_set<std::string> templateTypes)
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
		ss << "<";
		for (auto& type : templateTypes)
			ss << type << ", ";
		ss << ">";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::TemplateTypes;
	}
};
