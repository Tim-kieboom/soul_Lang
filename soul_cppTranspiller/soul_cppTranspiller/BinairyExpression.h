#pragma once
#include "SuperExpression.h"
#include "SyntaxTree_Operator.h"

class BinaryExpression : public SuperExpression
{
public:
	std::unique_ptr<SuperExpression> left;
	SyntaxTree_Operator operatorType;
	std::unique_ptr<SuperExpression> right;

	BinaryExpression(std::unique_ptr<SuperExpression> left, SyntaxTree_Operator opType, std::unique_ptr<SuperExpression> right)
		: left(std::move(left)), operatorType(opType), right(std::move(right))
	{
	}

	void print() const override
	{
		std::cout << "BinaryExpression(";
		left->print();
		std::cout << "operator::" << toString(operatorType);
		right->print();
		std::cout << ')';
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::BinairyExpression;
	}
};