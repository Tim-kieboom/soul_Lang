#pragma once
#include "BodyNode.h"
#include "ISyntaxNode.h"
#include "InitVariable.h"
#include "DefineTemplateTypes.h"
#include "SuperMainNodes.h"
#include "FuncDeclaration.h"

class FuncNode : public SuperMainNode
{
public:
	FuncDeclaration funcDecl;
	std::shared_ptr<BodyNode> body;
	std::shared_ptr<DefineTemplateTypes> templatesTypes;

	FuncNode() = default;

	FuncNode(FuncDeclaration& funcDecl, std::shared_ptr<BodyNode> body, std::shared_ptr<DefineTemplateTypes> templatesTypes)
		: funcDecl(funcDecl), body(body), templatesTypes(templatesTypes)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << '\n';
		if (templatesTypes)
			ss << templatesTypes->printToString() << '\n'; 

		ss << funcDecl.printToString();

		ss << body->printToString() << '\n';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::FuncNode;
	}
};