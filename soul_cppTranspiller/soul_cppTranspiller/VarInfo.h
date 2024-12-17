#pragma once
#include <string>
#include "TypeInfo.h"

struct VarInfo
{
	std::string name;
	TypeInfo type;
	bool isOption = false;
	bool isOnHeap = false;

	VarInfo() = default;

	VarInfo(const std::string& name, const TypeInfo& type, bool isOption = false)
		: name(name), type(type), isOption(isOption)
	{
	}
};