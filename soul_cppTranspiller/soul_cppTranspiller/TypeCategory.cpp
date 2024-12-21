#include "TypeCategory.h"

using namespace std;

TypeCategory getTypeCategory(PrimitiveType type)
{
	switch (type)
	{
	case PrimitiveType::i8:
	case PrimitiveType::i16:
	case PrimitiveType::i32:
	case PrimitiveType::i64:
		return TypeCategory::interger;

	case PrimitiveType::ui8:
	case PrimitiveType::ui16:
	case PrimitiveType::ui32:
	case PrimitiveType::ui64:
		return TypeCategory::unsignedInterger;

	case PrimitiveType::f32:
	case PrimitiveType::f64:
		return TypeCategory::floatingPoint;

	case PrimitiveType::bool_:
		return TypeCategory::boolean;

	case PrimitiveType::char_:
		return TypeCategory::character;

	case PrimitiveType::str:
	case PrimitiveType::c_str:
		return TypeCategory::text;

	case PrimitiveType::compile_dynamic:
		return TypeCategory::compile_dynamic;

	default:
		return TypeCategory::invalid;
	}
}

string toString(TypeCategory type)
{
	switch (type)
	{
	case TypeCategory::boolean:
		return "boolean";

	case TypeCategory::compile_dynamic:
		return "compile_dynamic";

	case TypeCategory::character:
		return "character";

	case TypeCategory::floatingPoint:
		return "foatingPoint";

	case TypeCategory::interger:
		return "interger";

	case TypeCategory::unsignedInterger:
		return "insignedInterger";

	case TypeCategory::text:
		return "text";

	default:
	case TypeCategory::invalid:
		return "invalid";
	}

}