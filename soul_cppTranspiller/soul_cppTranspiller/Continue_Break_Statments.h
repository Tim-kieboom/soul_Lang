#pragma once
#include "SuperStatement.h"

class ContinueStatment : public SuperStatement
{
public:
	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Continue()";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ContinueStatment;
	}
};

class BreakStatment : public SuperStatement
{
public:
	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Break()";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::BreakStatment;
	}
};
