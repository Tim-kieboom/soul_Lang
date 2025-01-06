#pragma once
#include <string>
#include <vector>
#include "ClassAccessLevel.h"

struct FieldsInfo
{
	std::string name = "";
	std::string stringedRawType = "";
	bool isProperty = false;
	ClassAccessLevel accessLevel = ClassAccessLevel::private_;

	FieldsInfo() = default;
	FieldsInfo(const std::string& name, const std::string& stringedRawType, ClassAccessLevel accessLevel, bool isProperty = false)
		: name(name), stringedRawType(stringedRawType), isProperty(isProperty), accessLevel(accessLevel)
	{
	}
};

struct Methode
{
	std::string methodeName;
	ClassAccessLevel accessLevel = ClassAccessLevel::private_;

	Methode() = default;
	Methode(std::string& methodeName, ClassAccessLevel accessLevel)
		: methodeName(methodeName), accessLevel(accessLevel)
	{
	}
};

struct ClassInfo
{
	std::string name;
	std::vector<FieldsInfo> publicFields;
	std::vector<Methode> methodes;

	bool isEqual(const ClassInfo& other) const
	{
		return name == other.name;
	}
};