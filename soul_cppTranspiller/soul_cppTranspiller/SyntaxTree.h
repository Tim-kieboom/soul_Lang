#pragma once
#include <vector>
#include "ISyntaxNode.h"
#include "InitVariable.h"
#include "FuncNode.h"
#include "ClassNode.h"

class SyntaxTree : public ISyntaxNode 
{
public:
	std::vector<std::shared_ptr<SuperStatement>> globalVariables;
	std::vector<std::shared_ptr<SuperMainNode>> funcsAndClasses;

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const
	{
		std::stringstream ss;
		for (const auto& var : globalVariables)
			ss << var->printToString() << '\n';

		for (const auto& funcOrClass : funcsAndClasses)
			ss << funcOrClass->printToString() << '\n';

		return ss.str();
	}


	SyntaxNodeId getId() const override 
	{
		return SyntaxNodeId::SyntaxTree;
	}
};

