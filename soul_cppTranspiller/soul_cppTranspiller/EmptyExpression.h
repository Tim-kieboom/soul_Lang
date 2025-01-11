#pragma once
#include "SuperExpression.h"

class EmptyExpression : public SuperExpression
{
	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		return "";
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::EmptyExpresion;
	}
};