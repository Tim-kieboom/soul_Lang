#include "PrimitiveType.h"

using namespace std;

PrimitiveType getPrimitiveType(const string& rawType)
{
	if (rawType == toString(PrimitiveType::c_str))
		return PrimitiveType::c_str;

	if (rawType == toString(PrimitiveType::str))
		return PrimitiveType::str;

	if (rawType == toString(PrimitiveType::char_))
		return PrimitiveType::char_;

	if (rawType == toString(PrimitiveType::bool_))
		return PrimitiveType::bool_;

	if (rawType == toString(PrimitiveType::compile_dynamic))
		return PrimitiveType::compile_dynamic;

	if (rawType == toString(PrimitiveType::f32))
		return PrimitiveType::f32;

	if (rawType == toString(PrimitiveType::f64))
		return PrimitiveType::f64;

	if (rawType == toString(PrimitiveType::i8))
		return PrimitiveType::i8;

	if (rawType == toString(PrimitiveType::i16))
		return PrimitiveType::i16;

	if (rawType == toString(PrimitiveType::i32))
		return PrimitiveType::i32;

	if (rawType == toString(PrimitiveType::i64))
		return PrimitiveType::i64;

	if (rawType == toString(PrimitiveType::ui8))
		return PrimitiveType::ui8;

	if (rawType == toString(PrimitiveType::ui16))
		return PrimitiveType::ui16;

	if (rawType == toString(PrimitiveType::ui32))
		return PrimitiveType::ui32;

	if (rawType == toString(PrimitiveType::ui64))
		return PrimitiveType::ui64;

	if (rawType == toString(PrimitiveType::none))
		return PrimitiveType::none;

	return PrimitiveType::invalid;
}

string toString(PrimitiveType type)
{
	switch (type)
	{
	case PrimitiveType::none:
		return "none";

	case PrimitiveType::compile_dynamic:
		return "compile_dynamic";

	case PrimitiveType::bool_:
		return "bool";

	case PrimitiveType::char_:
		return "char";

	case PrimitiveType::c_str:
		return "c_str";

	case PrimitiveType::str:
		return "str";

	case PrimitiveType::i8:
		return "i8";

	case PrimitiveType::i16:
		return "i16";

	case PrimitiveType::i32:
		return "i32";

	case PrimitiveType::i64:
		return "i64";

	case PrimitiveType::ui8:
		return "ui8";

	case PrimitiveType::ui16:
		return "ui16";

	case PrimitiveType::ui32:
		return "ui32";

	case PrimitiveType::ui64:
		return "ui64";

	case PrimitiveType::f32:
		return "f32";

	case PrimitiveType::f64:
		return "f64";
	}

	return "<invalid>";
}
