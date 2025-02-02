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
	ClassNode(std::string& className)
		: className(className)
	{
	}

	void addField(std::shared_ptr<FieldVariable> field)
	{
		fields.push_back(field);
	}

	void addMethode(std::shared_ptr<MethodeNode> methode)
	{
		methodes.push_back(methode);
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "className::" << className << "\n{\n";
		
		for (const std::shared_ptr<FieldVariable>& field : fields)
			ss << field->printToString() << '\n';
		
		for (const std::shared_ptr<MethodeNode>& methode : methodes)
			ss << methode->printToString() << '\n';
		
		ss << "\n}";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ClassNode;
	}
};