#pragma once
#include "TypeCategory.h"

enum class DuckType
{
	invalid,
	compile_dynamic,

	text,
	character,
	number
};

PrimitiveType DuckToPrimitive(DuckType type);
std::string toString(DuckType type);
DuckType getDuckType(PrimitiveType type);
DuckType getDuckType(TypeCategory type);