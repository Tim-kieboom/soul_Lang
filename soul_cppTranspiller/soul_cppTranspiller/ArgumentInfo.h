#pragma once
#include "TypeInfo.h"
#include "ArgumentType.h"

struct ArgumentInfo
{
	ArgumentType argType = ArgumentType::tk_default;
	TypeInfo valueType;

	std::string name;
	uint64_t argPosition = 0;
	std::string optionalValue;
	bool canBeMultiple = false;

	ArgumentInfo() = default;
	ArgumentInfo(ArgumentType argType, TypeInfo&& valueType, const char* name, uint64_t argPosition)
		: argType(argType),
		valueType(valueType),
		name(std::string(name)),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, TypeInfo& valueType, const std::string& name, uint64_t argPosition)
		: argType(argType),
		valueType(valueType),
		name(name),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, TypeInfo&& valueType, const char* name, uint64_t argPosition, bool canBeMultiple)
		: argType(argType),
		valueType(valueType),
		name(std::string(name)),
		canBeMultiple(canBeMultiple),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, TypeInfo& valueType, const std::string& name, uint64_t argPosition, bool canBeMultiple)
		: argType(argType),
		valueType(valueType),
		name(name),
		canBeMultiple(canBeMultiple),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, TypeInfo&& valueType, const char* name, uint64_t argPosition, bool canBeMultiple, const char* optionalValue)
		: argType(argType),
		valueType(valueType),
		name(std::string(name)),
		canBeMultiple(canBeMultiple),
		optionalValue(std::string(optionalValue)),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, TypeInfo&& valueType, const std::string& name, uint64_t argPosition, bool canBeMultiple, const std::string& optionalValue)
		: argType(argType),
		valueType(valueType),
		name(name),
		canBeMultiple(canBeMultiple),
		optionalValue(optionalValue),
		argPosition(argPosition)
	{
	}

	bool equals(const ArgumentInfo& other) const
	{
		return name == other.name &&
			argType == other.argType &&
			argPosition == argPosition &&
			valueType.equals(other.valueType) &&
			canBeMultiple == other.canBeMultiple;
	}

	bool about_equals(const ArgumentInfo& other) const
	{
		return argPosition == argPosition &&
			   valueType.equals(other.valueType);
	}
};

std::string toString(const ArgumentInfo& arg);