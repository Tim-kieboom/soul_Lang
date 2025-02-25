#pragma once
#include <string>

enum class SyntaxTree_Operator
{
	Invalid,

	Not,
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
	Pow,
	Root,
	Log,

	BitWise_OR,
	BitWise_AND,
	BitWise_XOR,
	Logical_OR,
	Logical_AND,
};
uint32_t getOperator_Precedence(const std::string& opType);
uint32_t getOperator_Precedence(SyntaxTree_Operator opType);

bool isOperator_booleanOp(SyntaxTree_Operator opType);

SyntaxTree_Operator getSyntax_Operator(const std::string& token);
std::string toString(SyntaxTree_Operator op);