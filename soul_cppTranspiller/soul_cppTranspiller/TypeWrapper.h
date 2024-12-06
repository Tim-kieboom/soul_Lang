#pragma once
#include <string>
enum class TypeWrapper
{
	refrence,
	default_,
	invalid,
	pointer,
	array_
};

std::string toString(TypeWrapper wrapper);