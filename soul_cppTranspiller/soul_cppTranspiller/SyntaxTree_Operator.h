#pragma once
#include <string>

enum class SyntaxTree_Operator
{
	Equals,
	NotEquals,
	IsSmaller,
	IsSmaller_Equals,
	IsBigger,
	IsBigger_Equals,

	add,
	sub,
	mul,
	div,
	modulo,

	BitWise_OR,
	BitWise_AND,
	BitWise_XOR,
	Logical_OR,
	Logical_AND,
	Logical_XOR,
};

std::string toString(SyntaxTree_Operator op);