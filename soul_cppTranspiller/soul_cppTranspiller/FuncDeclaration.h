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
		std::cout << printToString();
	}

	std::string printToString() const
	{
		std::stringstream ss;
		ss << returnType << ' ' << functionName << '(';
		for (uint32_t i = 0; i < args.size(); i++)
		{
			auto& arg = args.at(i);
			ss << "ArgumentType::" << toString(arg.argType);
			if (arg.isOptional)
				ss << "_optional";
			ss << " type::" << toString(arg.valueType) << ' ' << arg.argName;

			if ((uint64_t)i + 1 < args.size())
				ss << ", ";
		}
		ss << ")\n";
		return ss.str();
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

