#pragma once
#include "Assignment.h"
#include "InitVariable.h"

class CompileConstVariable : public SuperStatement
{
public:
	std::shared_ptr<InitializeVariable> init;

	CompileConstVariable() = default;
	CompileConstVariable(std::shared_ptr<InitializeVariable> init)
		: init(init)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "CompileConstVariable(" << init->printToString() << ')';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::CompileConstVariable;
	}
};