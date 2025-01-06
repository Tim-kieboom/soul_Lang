#pragma once
#include "SuperExpression.h"

class Literal : public SuperExpression
{
private:
	std::string value;

public:
	explicit Literal(const std::string& value) 
		: value(value)
	{
	}

	void print() const override
	{
		std::cout << "Literal(" << value << ")";
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Literal;
	}
};
