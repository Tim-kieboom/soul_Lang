#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class InitializeVariable : public SuperStatement
{
public:
    std::string type;
    std::string varName;

    InitializeVariable(const std::string& type, const std::string& varName)
        : type(type), varName(varName)
    {
    }

    void print() const override
    {
        std::cout << printToString();
    }

    std::string printToString() const
    {
        std::stringstream ss;
        ss << "InitializeVariable(" << varName << ", Type(" << type << "))";
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::InitializeVariable;
    }
};