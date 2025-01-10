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
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Literal(" << value << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Literal;
	}
};
