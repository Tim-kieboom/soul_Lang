#pragma once
#include <string>

enum class PrimitiveType
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

std::string toString(PrimitiveType type);
PrimitiveType getPrimitiveType(const std::string& rawType);