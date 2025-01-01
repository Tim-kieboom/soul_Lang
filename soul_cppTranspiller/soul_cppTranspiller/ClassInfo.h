#pragma once
#include <string>
#include <vector>
#include "TypeInfo.h"
#include "equalTools.h"

struct TypeInfo;

enum class ClassAccessType
{
	public_,
	private_
};

struct FieldsInfo
{
	std::string name;
	bool isProperty = false;
	ClassAccessType scope = ClassAccessType::private_;
};

struct Methode
{
	std::string methodeNames;
	ClassAccessType scope = ClassAccessType::private_;

	Methode() = default;
	Methode(std::string& methodeNames, ClassAccessType scope)
		: methodeNames(methodeNames), scope(scope)
	{
	}
};

struct ClassInfo
{
	std::string className;
	std::vector<TypeInfo> fieldsType;
	std::vector<FieldsInfo> fieldsInfo;
	std::vector<Methode> methodesNames;

	void setField(FieldsInfo& fieldInfo, TypeInfo& type)
	{
		fieldsType.push_back(type);
		fieldsInfo.push_back(fieldInfo);
	}

	Result<uint64_t> tryGetFieldIndex(const std::string& name, TypeInfo& type)
	{
		uint64_t i = 0;
		for (; i < fieldsInfo.size(); i++)
		{
			FieldsInfo& info = fieldsInfo.at(i);
			if (info.name == name)
				break;
		}

		return i;
	}

	bool equals(ClassInfo& other) const
	{
		return className == other.className;
	}
};