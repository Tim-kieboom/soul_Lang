#pragma once
#include <string>
#include <vector>
#include "TypeInfo.h"
#include "equalTools.h"

struct TypeInfo;

struct ClassInfo
{
	std::string className;
	std::vector<TypeInfo> fields;
	std::vector<std::string> methodesNames;

	bool equals(ClassInfo& other)
	{
		return className == other.className;
	}
};