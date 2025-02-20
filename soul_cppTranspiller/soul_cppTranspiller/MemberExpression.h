#pragma once
#include "SuperExpression.h"
#include "Variable.h"

class MemberExpression : public SuperExpression
{
public:
	std::shared_ptr<Variable> parent;
	std::shared_ptr<SuperExpression> memberExpression;

	MemberExpression(std::shared_ptr<Variable> parent, std::shared_ptr<SuperExpression> memberExpression)
		: parent(parent), memberExpression(memberExpression)
	{
	}

	void print() const 
	{
		std::cout << printToString();
	}

	std::string printToString() const 
	{
		std::stringstream ss;
		ss << "MemberExpression(parent: " << parent->printToString() << ", expression: " << memberExpression->printToString() << ")";
		return ss.str();
	}

	SyntaxNodeId getId() const 
	{
		return SyntaxNodeId::MemberExpression;
	}
};
