#pragma once
#include "InitVariable.h"
#include "ClassAccessLevel.h"

class FieldVariable : public SuperStatement
{
private:
    ClassAccessLevel accessLevel;
    std::unique_ptr<InitializeVariable> init;

public:
    FieldVariable(ClassAccessLevel accessLevel, std::unique_ptr<InitializeVariable> init)
        : accessLevel(accessLevel), init(std::move(init))
    {
    }

    void print() const override
    {
        std::cout << "FieldVariable(AccessLevel::" << toString(accessLevel) << ' ';
        init->print();
        std::cout << ')';
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::FieldNode;
    }
};