#pragma once
#include "SuperExpression.h"

class RangeExpression : public SuperExpression
{
public:
	std::shared_ptr<SuperExpression> begin;
	std::shared_ptr<SuperExpression> end;

	RangeExpression() = default;
	RangeExpression(std::shared_ptr<SuperExpression> begin, std::shared_ptr<SuperExpression> end)
		: begin(begin), end(end)
	{
	}
	
	void print() const override 
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Range(begin: " << begin->printToString() << ", end: " << end->printToString() << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::RangeExpression;
	}
};