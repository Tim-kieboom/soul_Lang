#pragma once
#include "SuperExpression.h"

class Refrence : public SuperExpression
{
public:
	std::shared_ptr<SuperExpression> expression;

	explicit Refrence(std::shared_ptr<SuperExpression>& expression)
		: expression(expression)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Refrence(" << expression->printToString() << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Refrence;
	}
};