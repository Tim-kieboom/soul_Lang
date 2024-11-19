#pragma once
#include <vector>

#include "VarInfo.h"
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

	FuncInfo(const char* funcName, Type returnType)
		: funcName(funcName), returnType(returnType)
	{
	}

	FuncInfo(const char* funcName, Type returnType, std::initializer_list<ArgumentInfo> args)
		: funcName(funcName), returnType(returnType)
	{
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
			this->args.push_back(arg);
	}

	FuncInfo(Type returnType, std::initializer_list<ArgumentInfo> args)
		: returnType(returnType)
	{
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
			this->args.push_back(arg);
	}
};