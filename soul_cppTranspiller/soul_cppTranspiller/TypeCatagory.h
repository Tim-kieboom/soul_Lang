#pragma once
#include "PrimitiveType.h"

enum class TypeCategory
{
	invalid,
	compile_dynamic,

	Int,
	Text,
	Bool,
	Char,
	Uint,
	Float
};

std::string toString(TypeCategory type);
TypeCategory getTypeCategory(PrimitiveType type);