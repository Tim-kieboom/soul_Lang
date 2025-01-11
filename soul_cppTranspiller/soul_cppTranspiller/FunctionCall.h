#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"


class FunctionCall : public SuperExpression
{
private:
    std::string funcName;
    std::string returnType;
    std::vector<std::shared_ptr<SuperExpression>> args;

public:
    FunctionCall() = default;

    FunctionCall(const std::string& funcName, const std::string& returnType, std::vector<std::shared_ptr<SuperExpression>>& args)
        : funcName(funcName), returnType(returnType), args(args)
    {
    }

    void print() const override
    {
        std::cout << printToString();
    }

    std::string printToString() const
    {
        std::stringstream ss;
        ss << "FunctionCall( " << funcName << "(";
        for(uint32_t i = 0; i < args.size(); i++)
        {
            ss << args.at(i)->printToString();
            if (i < args.size() - 1)
                ss << ", ";
        }
        ss << ") )";
        return ss.str();
    }

    SyntaxNodeId getId() const override
    {
        return SyntaxNodeId::FunctionCall;
    }
};