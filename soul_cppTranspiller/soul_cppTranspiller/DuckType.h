#pragma once
#include "PrimitiveType.h"
#include "TypeCatagory.h"

enum class DuckType
{
	invalid,
	compile_dynamic,
	compile_dynamic_withoutStr,

	Text,
	Char,
	Number
};

PrimitiveType DuckToPrimitive(DuckType type);
std::string toString(DuckType type);
DuckType getDuckType(PrimitiveType type);
DuckType getDuckType(TypeCategory type);