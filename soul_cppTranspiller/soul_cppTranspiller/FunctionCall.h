#pragma once
#include "SuperStatement.h"
#include "SuperExpression.h"
#include "TemplateType.h"
#include "TemplateType_ToType.h"
#include <map>

class FunctionCall : public SuperExpression
{
public:
    std::string funcName;
    std::string returnType;
    FuncDeclaration funcInfo;
    std::vector<std::shared_ptr<SuperExpression>> args;
    std::map<std::string, TemplateType_ToType> templateTypeDefines;

    FunctionCall() = default;

    FunctionCall(const std::string& funcName, const std::string& returnType, FuncDeclaration funcInfo, std::vector<std::shared_ptr<SuperExpression>>& args)
        : funcName(funcName), returnType(returnType), funcInfo(funcInfo), args(args)
    {
    }

    std::string getReturnType()
    {
        return returnType;
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