#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class InitializeVariable : public SuperStatement
{
public:
    std::string type;
    std::string varName;
    std::unique_ptr<SuperExpression> expression;

    InitializeVariable(const std::string& type, const std::string& varName, std::unique_ptr<SuperExpression> expression)
        : type(type), varName(varName), expression(std::move(expression))
    {
    }

    void print() const override
    {
        std::cout << "InitializeVariable(" << varName << " = ";
        expression->print();
        std::cout << ")";
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::InitializeVariable;
    }
};