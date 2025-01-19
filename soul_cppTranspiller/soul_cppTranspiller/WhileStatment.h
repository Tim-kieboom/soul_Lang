#pragma once
#include "BodyNode.h"
#include "SuperExpression.h"
#include "SuperConditionalStatment.h"

class WhileStatment : public SuperConditionalStatment
{
public:
	std::shared_ptr<SuperExpression> condition;
	std::shared_ptr<BodyNode> body;

	WhileStatment() {};
	WhileStatment(std::shared_ptr<SuperExpression> condition, std::shared_ptr<BodyNode> body)
		: condition(condition), body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "While(" << condition->printToString() << ")" << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::WhileStatment;
	}
};