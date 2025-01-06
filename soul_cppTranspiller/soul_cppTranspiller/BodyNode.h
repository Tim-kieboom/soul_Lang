#pragma once

#include "SuperStatement.h"
#include "ISyntaxNode.h"

class BodyNode : public ISyntaxNode
{
private:
	std::vector<std::unique_ptr<SuperStatement>> statements;

public:
	BodyNode() = default;

	void addStatment(std::unique_ptr<SuperStatement> statement)
	{
		statements.push_back(std::move(statement));
	}

	void print() const override
	{
		std::cout << "{\n";
		for(const std::unique_ptr<SuperStatement>& node : statements)
		{
			node->print();
			std::cout << ",\n";
		}
		std::cout << "\n}";
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::BodyNode;
	}
};