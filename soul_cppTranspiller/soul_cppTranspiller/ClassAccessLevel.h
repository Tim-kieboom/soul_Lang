#pragma once
#include <string>

enum class ClassAccessLevel
{
	invalid,

	priv,
	pub
};

std::string toString(ClassAccessLevel level);
ClassAccessLevel getClassAccessLevel(std::string& token);