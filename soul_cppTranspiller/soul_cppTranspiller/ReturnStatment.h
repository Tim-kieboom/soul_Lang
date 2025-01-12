#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class ReturnStatment : public SuperStatement
{
public:
	std::shared_ptr<SuperExpression> expression;

	ReturnStatment() = default;
	ReturnStatment(std::shared_ptr<SuperExpression>& expression)
		: expression(expression)
	{
	}

	void print() const override
	{
		std::cout << "";
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "ReturnStatment(" << expression->printToString() << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ReturnStatment;
	}
};
