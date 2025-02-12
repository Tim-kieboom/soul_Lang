#pragma once
#include <map>
#include <string>
#include <iostream>
#include <unordered_map>

#include "Result.h"
#include "ArgumentInfo.h"
#include "TemplateType.h"

struct FuncDeclaration
{
	std::string returnType;
	std::string functionName;
	std::vector<ArgumentInfo> args;
	std::unordered_map<std::string, ArgumentInfo> optionals;
	std::map<std::string, TemplateType> templateTypes;
	bool isForwardDeclared = true;
	bool hasReturnStament = false;
	bool isConstexpr = false;

	FuncDeclaration() = default;
	FuncDeclaration(const std::string& funcName, const std::string& returnType, std::initializer_list<ArgumentInfo> args_, std::map<std::string, TemplateType> templateTypes, bool isForwardDeclared = true, bool hasReturnStament = false, bool isConstexpr = false)
		: returnType(returnType), functionName(funcName), args(), isForwardDeclared(isForwardDeclared), templateTypes(templateTypes), hasReturnStament(hasReturnStament), isConstexpr(isConstexpr)
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
		
		if (isConstexpr)
			ss << "constexpr ";
		
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

	bool argsCompatible(const std::vector<ArgumentInfo>& other, const std::unordered_map<std::string, ArgumentInfo>& other_optionals, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, int64_t currentLine, ErrorInfo& error) const
	{
		uint64_t j = 0;
		for (uint64_t i = 0; i < other.size(); i++)
		{
			if (j >= args.size())
				return false;

			const ArgumentInfo& arg = args.at(j);
			const ArgumentInfo& other_arg = other.at(i);
			Result<void> result = arg.Compatible(other_arg, classStore, templateTypes, currentLine);
			if (result.hasError)
			{
				error = ErrorInfo("error at argument " + std::to_string(i + 1) + "\n" + result.error.message, currentLine);
				return false;
			}

			if (!arg.canBeMultiple)
				j++;
		}

		for (auto& pair : other_optionals)
		{
			auto& other_arg = pair.second;
			if (optionals.find(other_arg.argName) == optionals.end())
				return false;

			ArgumentInfo arg = optionals.at(other_arg.argName);
			Result<void> result = arg.Compatible(other_arg, classStore, templateTypes, currentLine);
			if (result.hasError)
			{
				error = ErrorInfo("error at optional: \'" + other_arg.argName + "\'\n" + result.error.message, currentLine);
				return false;
			}
		}

		return true;
	}

	bool argsCompatible(const std::vector<ArgumentInfo>& other_args, const std::vector<ArgumentInfo>& other_optionals, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, int64_t currentLine, ErrorInfo& error) const
	{
		if (other_args.size() < args.size())
			return false;

		uint64_t i = 0;
		for (uint64_t other_i = 0; other_i < other_args.size(); other_i++)
		{
			if (i >= args.size())
				return false;

			const ArgumentInfo& arg = args.at(i);
			const ArgumentInfo& other_arg = other_args.at(other_i);
			Result<void> result = arg.Compatible(other_arg, classStore, templateTypes, currentLine);
			if (result.hasError)
			{
				error = ErrorInfo("error at argument " + std::to_string(other_i+1) + "\n" + result.error.message, currentLine);
				return false;
			}

			if (!arg.canBeMultiple)
				i++;
		}

		for (uint64_t i = 0; i < other_optionals.size(); i++)
		{
			ArgumentInfo other_arg = other_optionals.at(i);
			if (optionals.find(other_arg.argName) == optionals.end())
				return false;

			ArgumentInfo arg = optionals.at(other_arg.argName);
			Result<void> result = arg.Compatible(other_arg, classStore, templateTypes, currentLine);
			if (result.hasError)
			{
				error = ErrorInfo("error at optional: \'" + other_arg.argName + "\'\n" + result.error.message, currentLine);
				return false;
			}
		}

		return true;
	}
};

