#pragma once
#include "Assignment.h"

class CompileConstVariable : public SuperStatement
{
public:
	std::string varName;
	std::string type;
	std::string value;

	CompileConstVariable() = default;
	CompileConstVariable(const std::string& varName, const std::string& type, const std::string& value)
		: varName(varName), type(type), value(value)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "CompileConstVariable( Type(" << type << ") " << varName << " = " << value << ')';
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::CompileConstVariable;
	}
};