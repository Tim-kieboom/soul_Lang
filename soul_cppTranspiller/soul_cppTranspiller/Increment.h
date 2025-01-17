#pragma once
#include <sstream>
#include "SuperExpression.h"
#include "Variable.h"

class Increment : public SuperExpression
{
public:
    std::shared_ptr<Variable> variable;
	bool isBeforeVar = false;
	bool isNegative = false;
	uint64_t incrementAmount = 1;

	Increment() = default;
	Increment(std::shared_ptr<Variable> variable, bool isBeforeVar, bool isNegative, uint64_t incrementAmount)
		: variable(variable), isBeforeVar(isBeforeVar), isNegative(isNegative), incrementAmount(incrementAmount)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "Increment(";
		if (isNegative)
			ss << '-';
		else
			ss << '+';

		ss << incrementAmount << ' ';

		if (isBeforeVar)
			ss << "beforeVar: ";
		else
			ss << "afterVar: ";
		ss << variable->printToString() << ")";

		return ss.str();
	}


	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::Increment;
	}
};