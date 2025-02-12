#include "DuckType.h"
#include "DuckType.h"

using namespace std;

DuckType getDuckType(PrimitiveType type)
{
	return getDuckType(getTypeCategory(type));
}

PrimitiveType DuckToPrimitive(DuckType type)
{
	switch (type)
	{
	case DuckType::Number:
		return PrimitiveType::f64;
	case DuckType::invalid:
		return PrimitiveType::invalid;
	case DuckType::compile_dynamic:
		return PrimitiveType::compile_dynamic;
	case DuckType::compile_dynamic_withoutStr:
		return PrimitiveType::compile_dynamic_withoutStr;
	case DuckType::Text:
		return PrimitiveType::str;
	case DuckType::Char:
		return PrimitiveType::char_;

	default:
		return PrimitiveType::invalid;
	}
}

DuckType getDuckType(TypeCategory type)
{
	switch (type)
	{
	case TypeCategory::compile_dynamic:
		return DuckType::compile_dynamic;

	case TypeCategory::compile_dynamic_withoutStr:
		return DuckType::compile_dynamic_withoutStr;

	case TypeCategory::Bool:
	case TypeCategory::Int:
	case TypeCategory::Float:
	case TypeCategory::Uint:
		return DuckType::Number;

	case TypeCategory::Text:
		return DuckType::Text;

	case TypeCategory::Char:
		return DuckType::Char;

	default:
	case TypeCategory::invalid:
		return DuckType::invalid;
	}
}

string toString(DuckType type)
{
	switch (type)
	{
	case DuckType::Number:
		return "Number";

	case DuckType::compile_dynamic:
		return "compile_dynamic";

	case DuckType::compile_dynamic_withoutStr:
		return "compile_dynamic_withoutStr";

	case DuckType::Char:
		return "Char";

	case DuckType::Text:
		return "Text";

	default:
	case DuckType::invalid:
		return "<invalid>";
	}

}