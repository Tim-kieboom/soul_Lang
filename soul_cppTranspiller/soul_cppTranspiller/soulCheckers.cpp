#include "soulCheckers.h"
#include "stringTools.h"


bool checkName(const std::string& name)
{
	if (name.empty())
		return false;

	//check if first char is number
	if (initListEquals(charNumbers, name.at(0)))
		return false;

	if (initListEquals(illigalNames, name))
		return false;

	return !string_contains(name, illigalNameSymbols);
}

DuckType getDuckType_fromValue(const std::string& value)
{
	if (checkValue(value, DuckType::character))
		return DuckType::character;

	if (checkValue(value, DuckType::number))
		return DuckType::number;

	if (checkValue(value, DuckType::text))
		return DuckType::text;

	return DuckType::invalid;
}

TypeCategory getTypeCategory_fromValue(const std::string& value)
{
	if (checkValue(value, TypeCategory::boolean))
		return TypeCategory::boolean;

	if (checkValue(value, TypeCategory::character))
		return TypeCategory::character;

	if (checkValue(value, TypeCategory::floatingPoint))
		return TypeCategory::floatingPoint;

	if (checkValue(value, TypeCategory::interger))
		return TypeCategory::interger;

	if (checkValue(value, TypeCategory::unsignedInterger))
		return TypeCategory::unsignedInterger;

	if (checkValue(value, TypeCategory::text))
		return TypeCategory::text;

	return TypeCategory::invalid;
}

bool checkValue(const std::string& value, const TypeInfo& type)
{
	if (type.isComplexType)
		return false;

	return checkValue(value, type.primType);
}

bool checkValue(const std::string& value, DuckType type)
{
	switch (type)
	{
	case DuckType::number:
		return checkValue(value, TypeCategory::boolean) ||
			checkValue(value, TypeCategory::interger) ||
			checkValue(value, TypeCategory::floatingPoint) ||
			checkValue(value, TypeCategory::unsignedInterger);

	case DuckType::compile_dynamic:
		return checkValue(value, TypeCategory::compile_dynamic);

	case DuckType::text:
		return checkValue(value, TypeCategory::text);

	case DuckType::character:
		return checkValue(value, TypeCategory::character);

	default:
	case DuckType::invalid:
		return false;
	}
}

bool checkValue(const std::string& value, PrimitiveType type)
{
	TypeCategory category = getTypeCategory(type);
	return checkValue(value, category);
}

bool checkValue(const std::string& value, TypeCategory category)
{
	switch (category)
	{
	case TypeCategory::compile_dynamic:
		return getDuckType_fromValue(value) != DuckType::invalid;
	case TypeCategory::text:
		return false;
	case TypeCategory::interger:
	{
		char* end;
		errno = 0;
		auto _ = strtoll(value.c_str(), &end, 10);
		return *end == '\0' && errno != ERANGE;
	}
	case TypeCategory::unsignedInterger:
	{
		char* end;
		errno = 0;
		auto _ = strtoull(value.c_str(), &end, 10);
		return *end == '\0' && errno != ERANGE;
	}
	case TypeCategory::boolean:
	{
		return value == "true" || value == "false";
	}
	case TypeCategory::character:
	{
		if (string_contains(value, '\\'))
		{
			if (value.length() > 4)
				return false;
		}
		else
		{
			if (value.length() > 3)
				return false;
		}

		return string_count(value, '\'') == 2;
	}
	case TypeCategory::floatingPoint:
	{
		char* end;
		errno = 0;
		auto _ = strtod(value.c_str(), &end);
		return *end == '\0' && errno != ERANGE;
	}
	default:
		return false;
	}
}