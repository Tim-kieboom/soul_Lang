#pragma once
#include <string>
#include <iostream>

#include "ArgumentInfo.h"

struct FuncDeclaration
{
	std::string returnType;
	std::string functionName;
	std::vector<ArgumentInfo> args;
	std::unordered_map<std::string, ArgumentInfo> optionals;

	FuncDeclaration() = default;
	FuncDeclaration(const std::string& funcName, const std::string& returnType, std::initializer_list<ArgumentInfo> args_)
		: returnType(returnType), functionName(funcName), args()
	{
		for (const ArgumentInfo& arg : args_)
		{
			if (arg.isOptional)
				pushOptional(arg);
			else
				args.push_back(arg);
		}
	}

	void pushOptional(ArgumentInfo arg)
	{
		arg.argPosition = optionals.size();
		optionals[arg.argName] = arg;
	}

	void pushOptionals(const std::vector<ArgumentInfo>& optionals_)
	{
		for (const ArgumentInfo& arg : optionals_)
			pushOptional(arg);
	}

	void print() const
	{
		std::cout << printToString();
	}

	std::string printToString() const
	{
		std::stringstream ss;
		ss << returnType << ' ' << functionName << '(';
		for (const ArgumentInfo& arg : args)
		{
			ss << "ArgumentType(" << toString(arg.argType) << ')';
			ss << " Type(" << toString(arg.valueType) << ") " << arg.argName << ", ";
		}

		for(const auto& kv : optionals)
		{
			const ArgumentInfo& arg = kv.second;
			ss << "ArgumentType(" << toString(arg.argType) << "_optional" << ')';
			ss << " Type(" << toString(arg.valueType) << ") " << arg.argName << ", ";
		}

		ss << ")\n";
		return ss.str();
	}

	bool argsCompatible(const std::vector<ArgumentInfo>& other, const std::vector<ArgumentInfo>& other_optionals, std::unordered_map<std::string, ClassInfo>& classStore) const
	{
		uint64_t j = 0;
		for (uint64_t i = 0; i < other.size(); i++)
		{
			if (j >= args.size())
				return false;

			const ArgumentInfo& arg = args.at(j);
			const ArgumentInfo& other_arg = other.at(i);
			if (!arg.Compatible(other_arg, classStore))
				return false;

			if (!arg.canBeMultiple)
				j++;
		}

		for (uint64_t i = 0; i < other_optionals.size(); i++)
		{
			ArgumentInfo other_arg = other_optionals.at(i);
			if (optionals.find(other_arg.argName) == optionals.end())
				return false;

			ArgumentInfo arg = optionals.at(other_arg.argName);
			if (!arg.Compatible(other_arg, classStore))
				return false;
		}

		return true;
	}
};

