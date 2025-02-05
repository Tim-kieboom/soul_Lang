#pragma once
#include "Nullable.h"
#include "BodyNode.h"
#include "Assignment.h"
#include "InitVariable.h"
#include "SuperExpression.h"
#include "SuperConditionalStatment.h"

class ForStatment : public SuperConditionalStatment
{
public:
	Nullable<std::shared_ptr<InitializeVariable>> initCounter;

	std::shared_ptr<SuperExpression> stopCondition;
	std::shared_ptr<SuperExpression> doAfterEachLoop;
	std::shared_ptr<BodyNode> body;

	ForStatment() = default;
	ForStatment(std::shared_ptr<SuperExpression> stopCondition, std::shared_ptr<SuperExpression> doAfterEachLoop, std::shared_ptr<BodyNode> body)
		: initCounter(), stopCondition(stopCondition), doAfterEachLoop(doAfterEachLoop), body(body)
	{
	}

	ForStatment(std::shared_ptr<InitializeVariable> initCounter, std::shared_ptr<SuperExpression> stopCondition, std::shared_ptr<SuperExpression> doAfterEachLoop, std::shared_ptr<BodyNode> body)
		: initCounter(initCounter), stopCondition(stopCondition), doAfterEachLoop(doAfterEachLoop), body(body)
	{
	}

	ForStatment(Nullable<std::shared_ptr<InitializeVariable>> initCounter, std::shared_ptr<SuperExpression> stopCondition, std::shared_ptr<SuperExpression> doAfterEachLoop, std::shared_ptr<BodyNode> body)
		: initCounter(initCounter), stopCondition(stopCondition), doAfterEachLoop(doAfterEachLoop), body(body)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "For(InitCounter: ";
		
		if (initCounter.isNull())
			ss << "<empty>, ";
		else
			ss << initCounter.value()->printToString() << ", ";

		ss <<"StopCondition: " << stopCondition->printToString() << ", DoAfterEachLoop: " << doAfterEachLoop->printToString() << ")" << body->printToString();
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::ForStatment;
	}
};
