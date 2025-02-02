#pragma once
#include "ClassAccessLevel.h"

std::string toString(ClassAccessLevel level)
{
	switch (level)
	{
	case ClassAccessLevel::priv:
		return "priv";
	case ClassAccessLevel::pub:
		return "pub";

	default:
		return "<invalid>";
	}
}

ClassAccessLevel getClassAccessLevel(std::string& token)
{
	if (token == toString(ClassAccessLevel::priv))
		return ClassAccessLevel::priv;

	if (token == toString(ClassAccessLevel::pub))
		return ClassAccessLevel::pub;

	return ClassAccessLevel::invalid;
}
