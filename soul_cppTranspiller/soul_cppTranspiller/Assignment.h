#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class Assignment : public SuperStatement
{
public: 
	std::shared_ptr<SuperExpression> setVariable;
	std::shared_ptr<SuperExpression> expression;

    Assignment() = default;

    Assignment(std::shared_ptr<SuperExpression> var, std::shared_ptr<SuperExpression> expression)
        : setVariable(var), expression(expression)
    {
    }

    void print() const override 
    {
        std::cout << printToString();
    }

    std::string printToString() const
    {
        std::stringstream ss;
        ss << "Assignment(" << setVariable->printToString() << " = " << expression->printToString() << ")";
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::Assignment;
    }
};