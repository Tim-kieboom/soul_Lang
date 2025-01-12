#pragma once

#include "SuperStatement.h"
#include "ISyntaxNode.h"

class BodyNode : public ISyntaxNode
{
private:
	std::vector<std::shared_ptr<SuperStatement>> statements;

public:
	BodyNode() = default;

	void addStatment(std::shared_ptr<SuperStatement> statement)
	{
		statements.push_back(statement);
	}

	void addStatment(std::vector<std::shared_ptr<SuperStatement>>& _statements)
	{
		for(auto& statement : _statements)
			statements.push_back(statement);
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "{\n";

		for (const std::shared_ptr<SuperStatement>& node : statements)
			ss << '\t' << node->printToString() << ",\n";

		ss << "\n}";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::BodyNode;
	}
};