#pragma once
#include "SuperMainNodes.h"
#include "FieldNode.h"
#include "MethodeNode.h"

class ClassNode : public SuperMainNode
{
public:
	std::string className;
	std::vector<std::shared_ptr<FieldVariable>> fields;
	std::vector<std::shared_ptr<MethodeNode>> methodes;

	ClassNode() = default;

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "classDef\n( className::" << className << "{";
		
		for (const std::shared_ptr<FieldVariable>& field : fields)
			ss << field->printToString();
		
		for (const std::shared_ptr<MethodeNode>& methode : methodes)
			ss << methode->printToString();
		
		ss << "\n})";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ClassNode;
	}
};