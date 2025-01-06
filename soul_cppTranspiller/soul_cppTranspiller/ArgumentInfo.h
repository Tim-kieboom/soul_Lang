#pragma once
#include <string>
#include "Type.h"

enum class ArgumentType
{
	invalid,

	default_,
	mut,
	out
};

ArgumentType getArgumentType(const std::string& token);
std::string toString(ArgumentType type);

struct ArgumentInfo
{
	bool isOptional;
	RawType valueType;
	std::string argName;
	ArgumentType argType;

	ArgumentInfo() = default;
	ArgumentInfo(bool isOptional, RawType valueType, std::string argName, ArgumentType argType)
		: isOptional(isOptional), valueType(valueType), argName(argName), argType(argType)
	{
	}

	bool equals(const ArgumentInfo& other) const
	{
		return isOptional == other.isOptional &&
			   valueType.isEqual(other.valueType) &&
			   argType == other.argType;
	}
};

std::string toString(ArgumentInfo& arg);
std::string toString(std::vector<ArgumentInfo>& args);
