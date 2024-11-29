#pragma once
#include <vector>

#include "nesting.hpp"
#include "ArgumentInfo.h"

struct FuncInfo
{
	const char* funcName = "";
	std::vector<ArgumentInfo> args;
	std::vector<Nesting> scope;
	Type returnType = Type::invalid;

	FuncInfo() = default;

	FuncInfo(const char* funcName)
		: funcName(funcName)
	{
		scope.emplace_back();
	}

	FuncInfo(const char* funcName, Type returnType)
		: funcName(funcName), returnType(returnType)
	{
		scope.emplace_back();
	}

	FuncInfo(const char* funcName, Type returnType, std::initializer_list<ArgumentInfo> args)
		: funcName(funcName), returnType(returnType)
	{
		scope.emplace_back();
		
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
		{
			auto varInfo = VarInfo(arg.name, arg.valueType, argType_isMutable(arg.argType), argType_isOptions(arg.argType));
			this->args.push_back(arg);
			this->scope.at(0).addVariable(varInfo);
		}
	}

	FuncInfo(Type returnType, std::initializer_list<ArgumentInfo> args)
		: returnType(returnType)
	{
		scope.emplace_back();
		
		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
		{
			auto varInfo = VarInfo(arg.name, arg.valueType, argType_isMutable(arg.argType), argType_isOptions(arg.argType));
			this->args.push_back(arg);
			this->scope.at(0).addVariable(varInfo);
		}
	}

	Nesting& addChildScope(Nesting* parent)
	{
		scope.emplace_back(Nesting::makeChild(parent));
		return scope.back();
	}
};