#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"

class InitializeVariable : public SuperStatement
{
public:
    std::string type;
    std::string varName;
    std::shared_ptr<SuperStatement> initStatment;

    InitializeVariable(const std::string& type, const std::string& varName, const std::shared_ptr<SuperStatement> initStatment)
        : type(type), varName(varName), initStatment(initStatment)
    {
    }

    void print() const override
    {
        std::cout << printToString();
    }

    std::string printToString() const
    {
        std::stringstream ss;
        ss << "InitializeVariable(" << varName << ", Type(" << type << "), " << initStatment->printToString() << ")";
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::InitializeVariable;
    }
};