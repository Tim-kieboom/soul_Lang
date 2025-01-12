#pragma once
#include "SuperExpression.h"

class Optional : public SuperExpression
{
public:
	std::string argName;
	std::shared_ptr<SuperExpression> value;

	explicit Optional(std::shared_ptr<SuperExpression>& value, const std::string& argName)
		: value(value), argName(argName)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Optional(" << argName << ": " << value->printToString() << ')';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Optional;
	}
};
