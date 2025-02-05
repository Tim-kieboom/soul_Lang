#pragma once
#include "Variable.h"
#include "SuperExpression.h"

class CopyExpression : public SuperExpression
{
public:
	std::string type;
	std::shared_ptr<SuperExpression> copyExpression;
	std::shared_ptr<SuperExpression> argument;

	CopyExpression() = default;
	CopyExpression(std::string type, std::shared_ptr<SuperExpression> copyExpression, std::shared_ptr<SuperExpression> argument)
		: type(type), copyExpression(copyExpression), argument(argument)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "CopyExpression( Type(" << type << ") " << copyExpression->printToString() << ", arguments: " + argument->printToString() + ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::CopyExpression;
	}
};
