#include "DuckType.h"

using namespace std;

DuckType getDuckType(PrimitiveType type)
{
	return getDuckType(getTypeCategory(type));
}

DuckType getDuckType(TypeCategory type)
{
	switch (type)
	{
	case TypeCategory::compile_dynamic:
		return DuckType::compile_dynamic;

	case TypeCategory::boolean:
	case TypeCategory::interger:
	case TypeCategory::floatingPoint:
	case TypeCategory::unsignedInterger:
		return DuckType::number;

	case TypeCategory::text:
		return DuckType::text;

	case TypeCategory::character:
		return DuckType::character;

	default:
	case TypeCategory::invalid:
		return DuckType::invalid;
	}
}

string toString(DuckType type)
{
	switch (type)
	{
	case DuckType::number:
		return "number";

	case DuckType::compile_dynamic:
		return "compile_dynamic";

	case DuckType::character:
		return "character";

	case DuckType::text:
		return "text";

	default:
	case DuckType::invalid:
		return "invalid";
	}

}