#include "TypeCatagory.h"

using namespace std;

TypeCategory getTypeCategory(PrimitiveType type)
{
	switch (type)
	{
	case PrimitiveType::i8:
	case PrimitiveType::i16:
	case PrimitiveType::i32:
	case PrimitiveType::i64:
		return TypeCategory::Int;

	case PrimitiveType::u8:
	case PrimitiveType::u16:
	case PrimitiveType::u32:
	case PrimitiveType::u64:
	case PrimitiveType::uSize:
		return TypeCategory::Uint;

	case PrimitiveType::f32:
	case PrimitiveType::f64:
		return TypeCategory::Float;

	case PrimitiveType::bool_:
		return TypeCategory::Bool;

	case PrimitiveType::char_:
		return TypeCategory::Char;

	case PrimitiveType::str:
	case PrimitiveType::c_str:
		return TypeCategory::Text;

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
	case TypeCategory::Bool:
		return "Bool";

	case TypeCategory::compile_dynamic:
		return "compile_dynamic";

	case TypeCategory::Char:
		return "Char";

	case TypeCategory::Float:
		return "Float";

	case TypeCategory::Int:
		return "Int";

	case TypeCategory::Uint:
		return "Uint";

	case TypeCategory::Text:
		return "Text";

	default:
	case TypeCategory::invalid:
		return "<invalid>";
	}

}