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
	
	std::shared_ptr<DefineTemplateTypes> templatesTypes;

	ClassNode() = default;
	ClassNode(std::string& className, std::shared_ptr<DefineTemplateTypes> templatesTypes)
		: className(className), templatesTypes(templatesTypes)
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
		ss << '\n';
		if (templatesTypes)
			ss << templatesTypes->printToString() << '\n';

		ss << "className::" << className;
		ss << "\n{\n";
		
		for (const std::shared_ptr<FieldVariable>& field : fields)
			ss << field->printToString() << '\n';
		
		for (const std::shared_ptr<MethodeNode>& methode : methodes)
			ss << methode->printToString() << '\n';
		
		ss << "\n}\n";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ClassNode;
	}
};