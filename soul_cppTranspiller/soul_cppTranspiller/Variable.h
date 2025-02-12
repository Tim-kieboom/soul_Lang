#pragma once
#include "SuperExpression.h"

class Variable : public SuperExpression
{
public:
	std::string varName;

	explicit Variable(const std::string& varName)
		: varName(varName)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Variable(" << varName << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Variable;
	}
};