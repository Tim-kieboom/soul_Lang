#pragma once
#include "FuncNode.h"
#include "ClassAccessLevel.h"

class MethodeNode : public SuperStatement
{
private:
    ClassAccessLevel accessLevel;
    std::shared_ptr<FuncNode> func;

public:
    MethodeNode(ClassAccessLevel accessLevel, std::shared_ptr<FuncNode> func)
        : accessLevel(accessLevel), func(func)
    {
    }

    void print() const override
    {
        std::cout << printToString();
    }

    std::string printToString() const override
    {
        std::stringstream ss;
        ss << "MethodeNode(AccessLevel::" << toString(accessLevel) << ' ' << func->printToString() << ')';
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::MethodeNode;
    }
};