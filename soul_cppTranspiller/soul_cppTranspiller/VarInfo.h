#pragma once
#include "Type.h"

struct VarInfo
{
	const char* name = "__foof__";
	Type type = Type::invalid;
	bool isMutable = true;
	bool isOption = false;

	VarInfo() = default;

	VarInfo(const char* name, Type type, bool isMutable)
		: name(name), type(type), isMutable(isMutable)
	{
	}

	VarInfo(const char* name, Type type, bool isMutable, bool isOption)
		: name(name), type(type), isMutable(isMutable), isOption(isOption)
	{
	}
};

