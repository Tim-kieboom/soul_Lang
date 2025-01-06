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
	std::unique_ptr<BodyNode> body;

	FuncNode() = default;

	FuncNode(FuncDeclaration& funcDecl, std::unique_ptr<BodyNode> body)
		: funcDecl(funcDecl), body(std::move(body))
	{
	}

	void print() const override
	{
		funcDecl.print();
		body->print();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::FuncNode;
	}
};