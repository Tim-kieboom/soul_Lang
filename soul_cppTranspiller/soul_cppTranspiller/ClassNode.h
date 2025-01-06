#pragma once
#include "SuperMainNodes.h"
#include "FieldNode.h"
#include "MethodeNode.h"

class ClassNode : public SuperMainNode
{
public:
	std::string className;
	std::vector<std::unique_ptr<FieldVariable>> fields;
	std::vector<std::unique_ptr<MethodeNode>> methodes;

	ClassNode() = default;

	void print() const override
	{
		std::cout << "classDef\n( className::" << className << "{";
		for (const std::unique_ptr<FieldVariable>& field : fields)
			field->print();
		for (const std::unique_ptr<MethodeNode>& methode : methodes)
			methode->print();
		std::cout << "\n})";
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ClassNode;
	}
};