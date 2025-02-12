#pragma once
#include "SuperExpression.h"

class ConstructArray : public SuperExpression
{
public:
	std::string type;
	std::shared_ptr<SuperExpression> amountElements;

	ConstructArray() = default;
	ConstructArray(std::string type, std::shared_ptr<SuperExpression> amountElements)
		: type(type), amountElements(amountElements)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "ConstructArray( Type(" << type << "), amountElements: " << amountElements->printToString() << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ConstructArray;
	}
};