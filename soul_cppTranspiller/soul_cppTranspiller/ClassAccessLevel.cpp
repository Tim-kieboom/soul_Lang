#pragma once
#include "ClassAccessLevel.h"

std::string toString(ClassAccessLevel level)
{
	switch (level)
	{
	case ClassAccessLevel::private_:
		return "private";
	case ClassAccessLevel::public_:
		return "public";

	default:
		return "<invalid>";
	}
}