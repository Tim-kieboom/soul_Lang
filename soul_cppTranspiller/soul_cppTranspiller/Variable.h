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
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Variable(" << value << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Variable;
	}
};