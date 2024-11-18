#pragma once
#include <vector>

#include "Type.h"
#include "VarInfo.h"
#include "ArgumentType.h"
#include "ArgumentInfo.h"

struct FuncInfo
{
	const char* funcName = "";
	std::vector<ArgumentInfo> args;
	std::vector<VarInfo> scope;
	Type returnType = Type::invalid;

	FuncInfo() = default;

	FuncInfo(const char* funcName)
		: funcName(funcName)
	{
	}

	FuncInfo(const char* funcName, std::initializer_list<ArgumentInfo> args)
		: funcName(funcName)
	{
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
			this->args.push_back(arg);
	}

	FuncInfo(std::initializer_list<ArgumentInfo> args)
	{
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
			this->args.push_back(arg);
	}
};