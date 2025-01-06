#pragma once
#include <vector>
#include "ISyntaxNode.h"
#include "InitVariable.h"
#include "FuncNode.h"
#include "ClassNode.h"

class SyntaxTree : public ISyntaxNode 
{
public:
	std::vector<std::shared_ptr<InitializeVariable>> globalVariables;
	std::vector<std::shared_ptr<SuperMainNode>> funcsAndClasses;

	void print() const override
	{
		for (const auto& var : globalVariables)
		{
			var->print();
			std::cout << '\n';
		}
		for (const auto& funcOrClass : funcsAndClasses)
		{
			funcOrClass->print();
			std::cout << '\n';
		}
	}

	SyntaxNodeId getId() const override 
	{
		return SyntaxNodeId::SyntaxTree;
	}
};

