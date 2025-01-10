#pragma once
#include "InitVariable.h"
#include "ClassAccessLevel.h"

class FieldVariable : public SuperStatement
{
private:
    ClassAccessLevel accessLevel;
    std::shared_ptr<InitializeVariable> init;

public:
    FieldVariable(ClassAccessLevel accessLevel, std::shared_ptr<InitializeVariable> init)
        : accessLevel(accessLevel), init(init)
    {
    }

    void print() const override
    {
        std::cout << printToString();
    }	
    
    std::string printToString() const override
    {
        std::stringstream ss;
        ss << "FieldVariable(AccessLevel::" << toString(accessLevel) << ' ' << init->printToString() << ')';
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::FieldNode;
    }
};