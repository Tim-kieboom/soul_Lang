#pragma once
#include <string>

enum class Type
{
	invalid,

	compile_dynamic,
	none,

	c_str,
	str,

	char_,
	bool_,

	i8,
	i16,
	i32,
	i64,

	ui8,
	ui16,
	ui32,
	ui64,

	f32,
	f64
};

enum class TypeCategory
{
	invalid,
	compile_dynamic,

	text,
	boolean,
	interger,
	character,
	floatingPoint,
	unsignedInterger
};

enum class DuckType
{
	invalid,
	compile_dynamic,

	text,
	character,
	number
};

TypeCategory getTypeCategory(Type type);

DuckType getDuckType(Type type);
DuckType getDuckType(TypeCategory type);

Type getType(const char* rawType);
Type getType(const std::string& rawType);

std::string toString(Type type);
std::string toString(void* value, Type type);