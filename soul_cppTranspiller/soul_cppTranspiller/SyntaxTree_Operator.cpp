#pragma once
#include "SyntaxTree_Operator.h"

std::string toString(SyntaxTree_Operator op)
{
	switch (op)
	{
	case SyntaxTree_Operator::Equals:
		return "=";
	case SyntaxTree_Operator::NotEquals:
		return "!=";
	case SyntaxTree_Operator::IsSmaller:
		return "<";
	case SyntaxTree_Operator::IsSmaller_Equals:
		return "<=";
	case SyntaxTree_Operator::IsBigger:
		return ">";
	case SyntaxTree_Operator::IsBigger_Equals:
		return ">=";

	case SyntaxTree_Operator::add:
		return "+";
	case SyntaxTree_Operator::sub:
		return "-";
	case SyntaxTree_Operator::mul:
		return "*";
	case SyntaxTree_Operator::div:
		return "/";
	case SyntaxTree_Operator::modulo:
		return "%";

	case SyntaxTree_Operator::BitWise_OR:
		return "|";
	case SyntaxTree_Operator::BitWise_AND:
		return "&";
	case SyntaxTree_Operator::BitWise_XOR:
		return "^";
	case SyntaxTree_Operator::Logical_OR:
		return "||";
	case SyntaxTree_Operator::Logical_AND:
		return "&&";
	case SyntaxTree_Operator::Logical_XOR:
		return "^^";

	default:
		return "<invalid>";
	}
}