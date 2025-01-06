#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class Assignment : public SuperStatement
{
private:
	std::string varName;
	std::unique_ptr<SuperExpression> expression;

public:
    Assignment() = default;

    Assignment(const std::string& varName, std::unique_ptr<SuperExpression> expression)
        : varName(varName), expression(std::move(expression)) 
    {
    }

    void print() const override 
    {
        std::cout << "Assignment(" << varName << " = ";
        expression->print();
        std::cout << ")";
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::Assignment;
    }
};