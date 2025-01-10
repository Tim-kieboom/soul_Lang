#pragma once
#include "BodyNode.h"
#include "SuperMainNodes.h"
#include "ISyntaxNode.h"
#include "InitVariable.h"
#include "FuncDeclaration.h"

class FuncNode : public SuperMainNode
{
public:
	FuncDeclaration funcDecl;
	std::shared_ptr<BodyNode> body;

	FuncNode() = default;

	FuncNode(FuncDeclaration& funcDecl, std::shared_ptr<BodyNode> body)
		: funcDecl(funcDecl), body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << funcDecl.printToString() << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::FuncNode;
	}
};