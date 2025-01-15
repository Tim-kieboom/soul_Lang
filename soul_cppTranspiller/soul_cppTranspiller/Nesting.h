#pragma once
#include <vector>
#include "Result.h"
#include "VarInfo.h"

struct Nesting
{
	uint32_t selfIndex = 0;
	int64_t parentIndex = -1;
	std::vector<VarInfo> vars;

	Nesting() = default;
	Nesting(uint32_t selfIndex)
		: selfIndex(selfIndex)
	{
	}
	Nesting(uint32_t selfIndex, std::initializer_list<VarInfo> vars)
		: selfIndex(selfIndex)
	{
		this->vars.reserve(vars.size());
		for (const auto& var : vars)
			this->vars.push_back(var);
	}
    Nesting(uint32_t selfIndex, std::vector<VarInfo>& vars)
        : selfIndex(selfIndex)
    {
        this->vars.reserve(vars.size());
        for (const auto& var : vars)
            this->vars.push_back(var);
    }

    static Nesting makeChild(Nesting* parent, std::vector<Nesting>& scope)
    {
        if (parent == nullptr)
            return Nesting(0);

        Nesting child = Nesting(parent->selfIndex + 1);
        child.parentIndex = parent->selfIndex;

        return child;
    }

    static Nesting makeChild(Nesting* parent, std::vector<Nesting>& scope, std::initializer_list<VarInfo> vars)
    {
        Nesting child = Nesting::makeChild(parent, scope);
        for (const auto& var : vars)
            child.vars.push_back(var);

        return child;
    }

    void addVariable(VarInfo& info)
    {
        vars.push_back(info);
    }

    Result<VarInfo*> tryGetVariable(const std::string& name, std::vector<Nesting>& scope, std::vector<VarInfo>& globalScope)
    {
        for (VarInfo& varInfo : vars)
        {
            if (varInfo.name == name)
                return &varInfo;
        }

        int64_t prevIndex = INT64_MIN;
        int64_t currentIndex = parentIndex;
        while (currentIndex >= 0)
        {
            if (prevIndex == currentIndex)
                throw std::exception("Nesting cirular ref issue");

            if (currentIndex == this->selfIndex)
                throw std::exception("Nesting ref's itself");

            Nesting& parent = scope.at(currentIndex);
            for (VarInfo& varInfo : parent.vars)
            {
                if (varInfo.name == name)
                    return &varInfo;
            }

            prevIndex = currentIndex;
            currentIndex = parent.parentIndex;
        }

        for (VarInfo& varInfo : globalScope)
        {
            if (varInfo.name == name)
                return &varInfo;
        }

        return ErrorInfo("tryGetVariable(): varNotFound", 0);
    }
};
