#pragma once
#include <string>

enum class ClassAccessLevel
{
	private_,
	public_
};

std::string toString(ClassAccessLevel level);