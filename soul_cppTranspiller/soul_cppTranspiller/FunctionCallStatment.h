#pragma once
#include "FunctionCall.h"
#include "SuperStatement.h"

class FunctionCallStatment : public SuperStatement
{
public:
	FunctionCall functionCall;

	FunctionCallStatment() = default;
	FunctionCallStatment(FunctionCall& functionCall)
		: functionCall(functionCall)
	{
	}

	void print() const override
	{
		functionCall.print();
	}

	std::string printToString() const override
	{
		return functionCall.printToString();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::FunctionCallStatment;
	}
};