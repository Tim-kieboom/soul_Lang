#pragma once
#include <string>

enum class SyntaxTree_Operator
{
	Invalid,

	Equals,
	NotEquals,
	IsSmaller,
	IsSmaller_Equals,
	IsBigger,
	IsBigger_Equals,

	Add,
	Sub,
	Mul,
	Div,
	Modulo,

	BitWise_OR,
	BitWise_AND,
	BitWise_XOR,
	Logical_OR,
	Logical_AND,
};
uint32_t getOperator_Priority(const std::string& opType);
uint32_t getOperator_Priority(SyntaxTree_Operator opType);

SyntaxTree_Operator getSyntax_Operator(const std::string& token);
std::string toString(SyntaxTree_Operator op);