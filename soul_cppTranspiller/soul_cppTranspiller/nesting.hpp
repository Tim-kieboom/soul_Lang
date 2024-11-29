#pragma once

#include "vector"
#include "Result.h"
#include "VarInfo.h"

struct Nesting
{
    std::vector<VarInfo> vars;
	std::vector<Nesting*> parents;

    Nesting() = default;
    Nesting(std::initializer_list<VarInfo> vars)
    {
        for (const auto& var : vars)
            this->vars.push_back(var);
    }

    static Nesting makeChild(Nesting* parent)
    {
        Nesting child;
        if (parent == nullptr)
            return child;

        child.parents.push_back(parent);
        for(const auto& ref : parent->parents)
            child.parents.push_back(ref);

        return child;
    }

    static Nesting makeChild(Nesting* parent, std::initializer_list<VarInfo> vars)
    {
        Nesting child = Nesting::makeChild(parent);
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

        for(const Nesting* parent : parents)
        {
            if (parent == nullptr)
                continue;

            for(const VarInfo& varInfo : parent->vars)
            {
                if (varInfo.name == name)
                    return varInfo;
            }
        }

        for (const VarInfo& varInfo : globalScope)
        {
            if (varInfo.name == name)
                return varInfo;
        }

        return ErrorInfo("tryGetVariable(): varNotFound", 0);
    }
};