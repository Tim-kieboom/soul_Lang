#pragma once
#include "BodyNode.h"
#include "SuperExpression.h"
#include "SuperConditionalStatment.h"

class ElseStatment : public SuperConditionalStatment
{
public:
	std::shared_ptr<BodyNode> body;

	ElseStatment() = default;
	ElseStatment(std::shared_ptr<BodyNode> body)
		: body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Else" << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ElseStatment;
	}
};

class ElseIfStatment : public SuperConditionalStatment
{
public:
	std::shared_ptr<BodyNode> body;
	std::shared_ptr<SuperExpression> expression;

	ElseIfStatment() = default;
	ElseIfStatment(std::shared_ptr<SuperExpression> expression)
		: expression(expression)
	{
	}

	ElseIfStatment(std::shared_ptr<SuperExpression> expression, std::shared_ptr<BodyNode> body)
		: expression(expression), body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Else If(" << expression->printToString() << ")" << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ElseIfStatment;
	}
};

class IfStatment : public SuperConditionalStatment
{
public:
	std::shared_ptr<BodyNode> body;
	std::shared_ptr<SuperExpression> expression;

	IfStatment() = default;
	IfStatment(std::shared_ptr<SuperExpression> expression)
		: expression(expression)
	{
	}

	IfStatment(std::shared_ptr<SuperExpression> expression, std::shared_ptr<BodyNode> body)
		: expression(expression), body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "If(" << expression->printToString() << ")" << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::IfStatment;
	}
};
