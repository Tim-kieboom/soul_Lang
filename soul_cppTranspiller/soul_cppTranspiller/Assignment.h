#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class Assignment : public SuperStatement
{
private:
	std::string varName;
	std::shared_ptr<SuperExpression> expression;

public:
    Assignment() = default;

    Assignment(const std::string& varName, std::shared_ptr<SuperExpression> expression)
        : varName(varName), expression(expression)
    {
    }

    void print() const override 
    {
        std::cout << printToString();
    }

    std::string printToString() const
    {
        std::stringstream ss;
        std::cout << "Assignment(" << varName << " = " << expression->printToString() << ")";
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::Assignment;
    }
};