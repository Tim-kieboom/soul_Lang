#pragma once
#include "PrimitiveType.h"

enum class TypeCategory
{
	invalid,
	compile_dynamic,
	compile_dynamic_withoutStr,

	Int,
	Text,
	Bool,
	Char,
	Uint,
	Float
};

std::string toString(TypeCategory type);
TypeCategory getTypeCategory(PrimitiveType type);