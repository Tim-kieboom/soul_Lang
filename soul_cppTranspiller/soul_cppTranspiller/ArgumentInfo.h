#pragma once
#include <string>
#include "Type.h"
#include "Result.h"
#include "SuperExpression.h"

enum class ArgumentType
{
	invalid,

	default_,
	mut,
	out
};

struct ArgumentInfo;

ArgumentType getArgumentType(const std::string& token);
std::string toString(ArgumentType type);
std::string toString(const ArgumentInfo& arg);
std::string toString(std::vector<ArgumentInfo>& args);

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
	 
	Result<void> Compatible(const ArgumentInfo& other, std::unordered_map<std::string, ClassInfo>& classStore, std::unordered_set<std::string>& templateTypes, int64_t currentLine) const
	{
		if (isOptional != other.isOptional)
		{
			return ErrorInfo("Argument1 is and Argument2 both need to be optional", currentLine);
		}

		if(argType == ArgumentType::out)
		{
			if (other.argType != ArgumentType::out)
				return ErrorInfo();

			return other.valueType.isEqual(valueType, classStore, templateTypes, currentLine);
		}

		Result<void> result = valueType.areTypeCompatible(other.valueType, classStore, templateTypes, currentLine);
		if (result.hasError)
			return result.error;
	
		return {};
	}
};
