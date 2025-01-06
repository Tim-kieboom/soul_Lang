#pragma once
#include "PrimitiveType.h"

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

std::string toString(TypeCategory type);
TypeCategory getTypeCategory(PrimitiveType type);