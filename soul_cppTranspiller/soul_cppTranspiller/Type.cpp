#include "Type.h"
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

TypeCategory getTypeCategory(Type type)
{
	switch (type)
	{
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
		return TypeCategory::interger;

	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
		return TypeCategory::unsignedInterger;

	case Type::f32:
	case Type::f64:
		return TypeCategory::floatingPoint;

	case Type::bool_:
		return TypeCategory::boolean;

	case Type::char_:
		return TypeCategory::character;

	case Type::str:
	case Type::c_str:
		return TypeCategory::text;

	default:
		return TypeCategory::invalid;
	}
}

DuckType getDuckType(Type type)
{
	return getDuckType(getTypeCategory(type));
}

DuckType getDuckType(TypeCategory type)
{
	switch (type)
	{
	case TypeCategory::boolean:
	case TypeCategory::interger:
	case TypeCategory::floatingPoint:
	case TypeCategory::unsignedInterger:
		return DuckType::number;

	case TypeCategory::text:
		return DuckType::text;

	case TypeCategory::invalid:
		return DuckType::invalid;

	case TypeCategory::character:
		return DuckType::character;
	}
}

Type getType(const char* rawType)
{
	return getType(string(rawType));
}

Type getType(const string& rawType)
{
	if (rawType == toString(Type::c_str))
		return Type::c_str;

	if (rawType == toString(Type::str))
		return Type::str;

	if (rawType == toString(Type::char_))
		return Type::char_;

	if (rawType == toString(Type::bool_))
		return Type::bool_;

	if (rawType == toString(Type::compile_dynamic))
		return Type::compile_dynamic;

	if (rawType == toString(Type::f32))
		return Type::f32;

	if (rawType == toString(Type::f64))
		return Type::f64;

	if (rawType == toString(Type::i8))
		return Type::i8;

	if (rawType == toString(Type::i16))
		return Type::i16;

	if (rawType == toString(Type::i32))
		return Type::i32;

	if (rawType == toString(Type::i64))
		return Type::i64;

	if (rawType == toString(Type::ui8))
		return Type::ui8;

	if (rawType == toString(Type::ui16))
		return Type::ui16;

	if (rawType == toString(Type::ui32))
		return Type::ui32;

	if (rawType == toString(Type::ui64))
		return Type::ui64;

	if (rawType == toString(Type::none))
		return Type::none;

	return Type::invalid;
}

string toString(Type type)
{
	switch (type)
	{
	case Type::none:
		return "none";

	case Type::compile_dynamic:
		return "compile_dynamic";

	case Type::bool_:
		return "bool";

	case Type::char_:
		return "char";

	case Type::c_str:
		return "c_str";

	case Type::str:
		return "str";

	case Type::i8:
		return "i8";

	case Type::i16:
		return "i16";

	case Type::i32:
		return "i32";

	case Type::i64:
		return "i64";

	case Type::ui8:
		return "ui8";

	case Type::ui16:
		return "ui16";

	case Type::ui32:
		return "ui32";

	case Type::ui64:
		return "ui64";

	case Type::f32:
		return "f32";

	case Type::f64:
		return "f64";
	}

	return "invalid";
}

string toString(void* value, Type type)
{
	stringstream ss;
	string boolStr = "";

	switch (type)
	{
	case Type::invalid:
	case Type::none:
		ss << "no type so value unknown";
		break;

	case Type::compile_dynamic:
		ss << "compile_dynamic type so value unknown";
		break;

	case Type::c_str:
		ss << (const char*)value;
		break;

	case Type::str:
		ss << (const char*)value;
		break;

	case Type::bool_:
		boolStr = ((bool)value) ? "true" : "false";
		ss << boolStr;
		break;

	case Type::i8:
		ss << (int16_t)((int8_t)value);
		break;

	case Type::i16:
		ss << (int16_t)value;
		break;

	case Type::i32:
		ss << (int32_t)value;
		break;

	case Type::i64:
		ss << (int64_t)value;
		break;

	case Type::ui8:
		ss << (uint8_t)value;
		break;

	case Type::ui16:
		ss << (uint16_t)value;
		break;

	case Type::ui32:
		ss << (uint32_t)value;
		break;

	case Type::ui64:
		ss << (uint64_t)value;
		break;

	case Type::f32:
		ss << *(float*)&value;
		break;

	case Type::f64:
		ss << *(double*)&value;
		break;
	}

	return ss.str();
}
