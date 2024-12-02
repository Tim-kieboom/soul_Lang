#pragma once

#include "vector"
#include "Result.h"
#include "VarInfo.h"

struct Nesting
{
    uint32_t selfIndex;
    std::vector<VarInfo> vars;
    std::vector<Nesting>* scope;
	std::vector<uint32_t> parents;

    Nesting() = default;

    Nesting(std::vector<Nesting>* scope, uint32_t selfIndex)
        : scope(scope), selfIndex(selfIndex)
    {
    }

    Nesting(std::vector<Nesting>* scope, uint32_t selfIndex, std::initializer_list<VarInfo> vars)
        : scope(scope), selfIndex(selfIndex) 
    {
        for (const auto& var : vars)
            this->vars.push_back(var);
    }

    static Nesting makeChild(Nesting* parent, std::vector<Nesting>& scope)
    {
        if (parent == nullptr)
            return Nesting(&scope, 0);

        Nesting child = Nesting(&scope, parent->selfIndex+1);
        child.parents.push_back(parent->selfIndex);
        for(const auto& ref : parent->parents)
            child.parents.push_back(ref);

        return child;
    }

    static Nesting makeChild(Nesting* parent, std::vector<Nesting>& scope, std::initializer_list<VarInfo> vars)
    {
        Nesting child = Nesting::makeChild(parent, scope);
        for(const auto& var : vars)
            child.vars.push_back(var);

        return child;
    }

    void addVariable(VarInfo& info)
    {
        vars.push_back(info);
    }

    Result<VarInfo> tryGetVariable(const std::string& name, std::vector<VarInfo>& globalScope)
    {
        for(const VarInfo& varInfo : vars)
        {
            if (varInfo.name == name)
                return varInfo;
        }

        if (scope != nullptr)
        {
            Result<VarInfo> varResult = searchParents(name);
            if (!varResult.hasError)
                return varResult.value();
        }

        for (const VarInfo& varInfo : globalScope)
        {
            if (varInfo.name == name)
                return varInfo;
        }

        return ErrorInfo("tryGetVariable(): varNotFound", 0);
    }

private:
    Result<VarInfo> searchParents(const std::string& name)
    {
        for (const uint32_t& parent : parents)
        {
            for (const VarInfo& varInfo : scope->at(parent).vars)
            {
                if (varInfo.name == name)
                    return varInfo;
            }
        }

        return ErrorInfo("not found", 0);
    }
};