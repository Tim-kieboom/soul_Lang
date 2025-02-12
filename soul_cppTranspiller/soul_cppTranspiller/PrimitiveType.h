#pragma once
#include <string>

enum class PrimitiveType
{
	invalid,

	compile_dynamic,
	compile_dynamic_withoutStr,
	none,

	str,

	char_,
	bool_,

	i8,
	i16,
	i32,
	i64,

	u8,
	u16,
	u32,
	u64,
	uSize,

	f32,
	f64
};

std::string toString(PrimitiveType type);
PrimitiveType getPrimitiveType(const std::string& rawType);