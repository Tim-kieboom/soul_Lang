#pragma once

#include "SuperExpression.h"

class Variable : public SuperExpression
{
private:
	std::string value;

public:
	explicit Variable(const std::string& value)
		: value(value)
	{
	}

	void print() const override
	{
		std::cout << "Variable(" << value << ")";
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Variable;
	}
};