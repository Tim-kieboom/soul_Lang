#pragma once
#include <string>
enum class TypeWrapper
{
	invalid,

	refrence,
	default_,
	pointer,
	array_
};

std::string toString(TypeWrapper wrapper);
TypeWrapper getTypeWrapper(const std::string& token);