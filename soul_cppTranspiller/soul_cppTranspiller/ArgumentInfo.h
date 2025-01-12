#pragma once
#include <string>
#include "Type.h"
#include "SuperExpression.h"

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
	bool isOptional = false;
	std::shared_ptr<SuperExpression> defaultValue;

	RawType valueType;
	std::string argName;
	ArgumentType argType = ArgumentType::invalid;

	uint64_t argPosition = 0;
	bool canBeMultiple = false;

	ArgumentInfo() = default;
	ArgumentInfo(bool isOptional, RawType valueType, std::string argName, ArgumentType argType, uint64_t argPosition, bool canBeMultiple = false)
		: isOptional(isOptional), valueType(valueType), argName(argName), argType(argType), argPosition(argPosition), canBeMultiple(canBeMultiple)
	{
	}

	bool Compatible(const ArgumentInfo& other, std::unordered_map<std::string, ClassInfo>& classStore) const
	{
		if (isOptional != other.isOptional)
			return false;
			
		if (argType == ArgumentType::out && other.argType != ArgumentType::out)
			return false;

		if (valueType.areTypeCompatible(other.valueType, classStore, 0).hasError)
			return false;

		return true;
	}
};

std::string toString(ArgumentInfo& arg);
std::string toString(std::vector<ArgumentInfo>& args);
