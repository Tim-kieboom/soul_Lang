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

bool checkValue(const std::string& value, DuckType type)
{
	switch (type)
	{
	case DuckType::number:
		return checkValue(value, TypeCategory::floatingPoint) || checkValue(value, TypeCategory::interger) || checkValue(value, TypeCategory::unsignedInterger) || checkValue(value, TypeCategory::boolean);

	case DuckType::compile_dynamic:
		return checkValue(value, TypeCategory::compile_dynamic);

	case DuckType::text:
		return checkValue(value, TypeCategory::text);

	case DuckType::character:
		return checkValue(value, TypeCategory::character);

	case DuckType::invalid:
		return false;
	}
}

bool checkValue(const std::string& value, Type type)
{
	TypeCategory category = getTypeCategory(type);
	return checkValue(value, category);
}

bool checkValue(const std::string& value, TypeCategory category)
{
	switch (category)
	{
	case TypeCategory::compile_dynamic:
	case TypeCategory::text:
		return true;
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
		return value.length() == 3 && string_count(value, '\'') == 2;
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