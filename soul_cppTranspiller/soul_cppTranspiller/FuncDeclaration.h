#pragma once
#include <string>
#include <iostream>

#include "ArgumentInfo.h"

struct FuncDeclaration
{
	std::string returnType;
	std::string functionName;
	std::vector<ArgumentInfo> args;

	void print() const
	{
		std::cout << returnType << ' ' << functionName << '(';
		for(uint32_t i = 0; i < args.size(); i++)
		{
			auto& arg = args.at(i);
			std::cout << "ArgumentType::" << toString(arg.argType);
			if (arg.isOptional)
				std::cout << "_optional";
			std::cout << " type::" << toString(arg.valueType) << ' ' << arg.argName;

			if ((uint64_t)i + 1 < args.size())
				std::cout << ", ";
		}
		std::cout << ")\n";
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

