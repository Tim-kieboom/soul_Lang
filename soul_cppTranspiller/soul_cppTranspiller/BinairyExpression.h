#pragma once
#include "SuperExpression.h"
#include "SyntaxTree_Operator.h"

class BinaryExpression : public SuperExpression
{
public:
	std::shared_ptr<SuperExpression> left;
	SyntaxTree_Operator operatorType;
	std::shared_ptr<SuperExpression> right;

	BinaryExpression(std::shared_ptr<SuperExpression> left, SyntaxTree_Operator opType, std::shared_ptr<SuperExpression> right)
		: left(left), operatorType(opType), right(right)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "BinaryExpression(" << left->printToString() << ' ' << toString(operatorType) << ' ' << right->printToString() << ')';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::BinairyExpression;
	}
};