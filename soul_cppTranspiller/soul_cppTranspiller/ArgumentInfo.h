#pragma once

#include "Type.h"
#include "ArgumentType.h"

struct ArgumentInfo
{
	ArgumentType argType = ArgumentType::tk_default;
	Type valueType = Type::invalid;
	const char* name = "foo";
	bool canBeMultiple = false;
	const char* optionalValue = nullptr;
	uint64_t argPosition = 0;

	ArgumentInfo() = default;
	ArgumentInfo(ArgumentType argType, Type valueType, const char* name, uint64_t argPosition)
		: argType(argType),
		valueType(valueType),
		name(name),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, Type valueType, const char* name, uint64_t argPosition, bool canBeMultiple)
		: argType(argType),
		valueType(valueType),
		name(name),
		canBeMultiple(canBeMultiple),
		argPosition(argPosition)
	{
	}

	ArgumentInfo(ArgumentType argType, Type valueType, const char* name, uint64_t argPosition, bool canBeMultiple, const char* optionalValue)
		: argType(argType),
		valueType(valueType),
		name(name),
		canBeMultiple(canBeMultiple),
		optionalValue(optionalValue),
		argPosition(argPosition)
	{
	}
};