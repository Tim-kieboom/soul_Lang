#pragma once
#include <string>
#include <vector>

#include "Nesting.h"
#include "ArgumentInfo.h"

struct FuncInfo
{
	std::string funcName;
	std::vector<ArgumentInfo> args;
	std::vector<Nesting> scope;
	TypeInfo returnType;
	uint32_t currentNestingIndex = 0;

	FuncInfo() = default;

	FuncInfo(const std::string& funcName)
		: funcName(funcName)
	{
		scope.emplace_back(Nesting(0));
	}

	FuncInfo(const char* funcName, TypeInfo& returnType)
		: funcName(funcName), returnType(returnType)
	{
		scope.emplace_back(Nesting(0));
	}

	FuncInfo(const char* funcName, TypeInfo& returnType, std::initializer_list<ArgumentInfo> args)
		: funcName(funcName), returnType(returnType)
	{
		scope.emplace_back(Nesting(0));

		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
		{
			VarInfo varInfo = VarInfo(arg.name, arg.valueType, argType_isMutable(arg.argType));
			this->args.push_back(arg);
			this->scope.at(0).addVariable(varInfo);
		}
	}

	FuncInfo(const char* funcName, TypeInfo&& returnType, std::initializer_list<ArgumentInfo> args)
		: funcName(funcName), returnType(returnType)
	{
		scope.emplace_back(Nesting(0));

		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
		{
			VarInfo varInfo = VarInfo(arg.name, arg.valueType, argType_isMutable(arg.argType));
			this->args.push_back(arg);
			this->scope.at(0).addVariable(varInfo);
		}
	}

	FuncInfo(TypeInfo& returnType, std::initializer_list<ArgumentInfo> args)
		: returnType(returnType)
	{
		scope.emplace_back(Nesting(0));

		this->args.reserve(args.size());
		for (const ArgumentInfo& arg : args)
		{
			VarInfo varInfo = VarInfo(arg.name, arg.valueType, argType_isMutable(arg.argType));
			this->args.push_back(arg);
			this->scope.at(0).addVariable(varInfo);
		}
	}

	bool argsAboutEquals(const std::vector<ArgumentInfo>& other) const
	{
		if (args.size() != other.size())
			return false;

		for(uint64_t i = 0; i < args.size(); i++)
		{
			const ArgumentInfo& one = args.at(i);
			const ArgumentInfo& two = other.at(i);
			if(!one.about_equals(two))
				return false;
		}

		return true;
	}

	bool argsEquals(const std::vector<ArgumentInfo>& other) const
	{
		if (args.size() != other.size())
			return false;

		for (uint64_t i = 0; i < args.size(); i++)
		{
			const ArgumentInfo& one = args.at(i);
			const ArgumentInfo& two = other.at(i);
			if (!one.equals(two))
				return false;
		}

		return true;
	}
};