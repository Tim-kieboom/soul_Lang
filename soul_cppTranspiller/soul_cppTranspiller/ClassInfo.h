#pragma once
#include <string>
#include <vector>
#include <map>

#include "Result.h"
#include "ClassAccessLevel.h"
#include "TemplateType.h"

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
	std::map<std::string, FieldsInfo> fields;
	std::map<std::string, MethodeDecleration> methodes;
	std::map<std::string, TemplateType> templateTypes;

	ClassInfo() = default;
	ClassInfo(std::string& name, std::map<std::string, TemplateType>& templateTypes)
		: name(name), templateTypes(templateTypes)
	{
	}

	void addMethode(MethodeDecleration& methode)
	{
		methodes[methode.methodeName] = methode;
	}

	void addField(FieldsInfo field)
	{
		fields[field.name] = field;
	}

	Result<FieldsInfo> getField(std::string& name, uint32_t currentLine)
	{
		if (!isField(name))
			return ErrorInfo("isNotField", currentLine);

		return fields[name];
	}

	Result<MethodeDecleration> getMethode(std::string& name, std::string& className, uint32_t currentLine)
	{
		if (!isMethode(name, className))
			return ErrorInfo("isNotMethode", currentLine);

		return methodes[className + "#" + name];
	}

	bool isField(std::string& name)
	{
		return fields.find(name) != fields.end();
	}

	bool isMethode(std::string& name, std::string& className)
	{
		return methodes.find(className + "#" + name) != methodes.end();
	}

	bool isEqual(const ClassInfo& other) const
	{
		return name == other.name;
	}
};