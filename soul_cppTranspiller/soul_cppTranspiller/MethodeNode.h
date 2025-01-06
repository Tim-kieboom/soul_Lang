#pragma once
#include "FuncNode.h"
#include "ClassAccessLevel.h"

class MethodeNode : public SuperStatement
{
private:
    ClassAccessLevel accessLevel;
    std::unique_ptr<FuncNode> func;

public:
    MethodeNode(ClassAccessLevel accessLevel, std::unique_ptr<FuncNode> func)
        : accessLevel(accessLevel), func(std::move(func))
    {
    }

    void print() const override
    {
        std::cout << "MethodeNode(AccessLevel::" << toString(accessLevel) << ' ';
        func->print();
        std::cout << ')';
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::MethodeNode;
    }
};