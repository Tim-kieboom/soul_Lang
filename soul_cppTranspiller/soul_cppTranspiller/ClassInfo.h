#pragma once
#include <string>
#include <vector>
#include <unordered_set>

#include "Result.h"
#include "ClassAccessLevel.h"

struct FieldsInfo
{
	std::string name;
	std::string stringRawType;
	bool isProperty = false;
	ClassAccessLevel accessLevel = ClassAccessLevel::priv;

	FieldsInfo() = default;
	FieldsInfo(std::string& name, std::string& stringRawType, ClassAccessLevel accessLevel, bool isProperty = false)
		: name(name), stringRawType(stringRawType), isProperty(isProperty), accessLevel(accessLevel)
	{
	}
};

struct ArgumentInfo;

struct MethodeDecleration
{
	std::string methodeName;
	std::vector<ArgumentInfo> args;
	ClassAccessLevel accessLevel = ClassAccessLevel::priv;

	MethodeDecleration() = default;
	MethodeDecleration(std::string& methodeName, std::vector<ArgumentInfo>& args, ClassAccessLevel accessLevel)
		: methodeName(methodeName), args(args), accessLevel(accessLevel)
	{
	}
};

struct ClassInfo
{
	std::string name;
	std::vector<FieldsInfo> fields;
	std::vector<MethodeDecleration> methodes;
	std::unordered_set<std::string> templateTypes;

	ClassInfo() = default;
	ClassInfo(std::string& name, std::unordered_set<std::string>& templateTypes)
		: name(name), templateTypes(templateTypes)
	{
	}

	void addMethode(MethodeDecleration& methode)
	{
		methodes.push_back(methode);
	}

	void addField(FieldsInfo field)
	{
		fields.push_back(field);
	}

	Result<FieldsInfo> isField(std::string& name, uint32_t currentLine)
	{
		for (FieldsInfo& field : fields)
		{
			if (field.name == name)
				return field;
		}

		return ErrorInfo("isNotField", currentLine);
	}

	bool isEqual(const ClassInfo& other) const
	{
		return name == other.name;
	}
};