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

std::string toString(DuckType type);
DuckType getDuckType(PrimitiveType type);
DuckType getDuckType(TypeCategory type);