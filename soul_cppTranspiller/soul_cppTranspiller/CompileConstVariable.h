#pragma once
#include "Assignment.h"
#include "InitVariable.h"

class CompileConstVariable : public SuperStatement
{
public:
	std::shared_ptr<InitializeVariable> init;
	std::shared_ptr<Assignment> assign;

	CompileConstVariable() = default;
	CompileConstVariable(std::shared_ptr<InitializeVariable> init, std::shared_ptr<Assignment> assign)
		: init(init), assign(assign)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "CompileConstVariable(" << init->printToString() << ", " << assign->printToString() << ')';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::CompileConstVariable;
	}
};