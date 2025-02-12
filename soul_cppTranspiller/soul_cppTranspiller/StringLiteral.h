#pragma once
#include "SuperExpression.h"

class StringLiteral : public SuperExpression
{
public:
	std::string value;

	explicit StringLiteral(const std::string& value)
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
		ss << "StringLiteral(" << value << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::StringLiteral;
	}
};