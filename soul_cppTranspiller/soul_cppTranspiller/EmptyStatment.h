#pragma once
#include "SuperStatement.h"

class EmptyStatment : public SuperStatement 
{
public:
	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "<EmptyStatment>";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::EmptyStatment;
	}
};