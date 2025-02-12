#pragma once
#include "SyntaxTree_Operator.h"

uint32_t getOperator_Precedence(const std::string& opType)
{
	return getOperator_Precedence(getSyntax_Operator(opType));
}

uint32_t getOperator_Precedence(SyntaxTree_Operator opType)
{
	switch (opType)
	{
	case SyntaxTree_Operator::Not:
		return 8;

	case SyntaxTree_Operator::Log:
	case SyntaxTree_Operator::Pow:
	case SyntaxTree_Operator::Root:
		return 7;

	case SyntaxTree_Operator::Mul:
	case SyntaxTree_Operator::Div:
	case SyntaxTree_Operator::Modulo:
		return 6;

	case SyntaxTree_Operator::Add:
	case SyntaxTree_Operator::Sub:
		return 5;

	case SyntaxTree_Operator::IsSmaller:
	case SyntaxTree_Operator::IsSmaller_Equals:
	case SyntaxTree_Operator::IsBigger:
	case SyntaxTree_Operator::IsBigger_Equals:
		return 4;

	case SyntaxTree_Operator::Equals:
	case SyntaxTree_Operator::NotEquals:
		return 3;

	case SyntaxTree_Operator::BitWise_OR:
	case SyntaxTree_Operator::BitWise_AND:
	case SyntaxTree_Operator::BitWise_XOR:
		return 2;

	case SyntaxTree_Operator::Logical_OR:
	case SyntaxTree_Operator::Logical_AND:
		return 1;

	default:
	case SyntaxTree_Operator::Invalid:
		return 0;
	}
}

bool isOperator_booleanOp(SyntaxTree_Operator opType)
{
	switch (opType)
	{
		case SyntaxTree_Operator::Not:
		case SyntaxTree_Operator::IsBigger:
		case SyntaxTree_Operator::NotEquals:
		case SyntaxTree_Operator::IsSmaller:
		case SyntaxTree_Operator::Logical_OR:
		case SyntaxTree_Operator::Logical_AND:
		case SyntaxTree_Operator::IsBigger_Equals:
		case SyntaxTree_Operator::IsSmaller_Equals:
			return true;

		case SyntaxTree_Operator::Log:
		case SyntaxTree_Operator::Pow:
		case SyntaxTree_Operator::Mul:
		case SyntaxTree_Operator::Div:
		case SyntaxTree_Operator::Add:
		case SyntaxTree_Operator::Sub:
		case SyntaxTree_Operator::Root:
		case SyntaxTree_Operator::Modulo:
		case SyntaxTree_Operator::Invalid:
		case SyntaxTree_Operator::BitWise_OR:
		case SyntaxTree_Operator::BitWise_AND:
		case SyntaxTree_Operator::BitWise_XOR:
		default:
			return false;
	}
}

SyntaxTree_Operator getSyntax_Operator(const std::string& token)
{
	if (token == toString(SyntaxTree_Operator::Not))
		return SyntaxTree_Operator::Not;

	if (token == toString(SyntaxTree_Operator::Equals))
		return SyntaxTree_Operator::Equals;

	if (token == toString(SyntaxTree_Operator::NotEquals))
		return SyntaxTree_Operator::NotEquals;

	if (token == toString(SyntaxTree_Operator::IsSmaller))
		return SyntaxTree_Operator::IsSmaller;

	if (token == toString(SyntaxTree_Operator::IsSmaller_Equals))
		return SyntaxTree_Operator::IsSmaller_Equals;

	if (token == toString(SyntaxTree_Operator::IsSmaller_Equals))
		return SyntaxTree_Operator::IsSmaller_Equals;

	if (token == toString(SyntaxTree_Operator::IsBigger))
		return SyntaxTree_Operator::IsBigger;

	if (token == toString(SyntaxTree_Operator::IsBigger_Equals))
		return SyntaxTree_Operator::IsBigger_Equals;


	if (token == toString(SyntaxTree_Operator::Add))
		return SyntaxTree_Operator::Add;

	if (token == toString(SyntaxTree_Operator::Sub))
		return SyntaxTree_Operator::Sub;

	if (token == toString(SyntaxTree_Operator::Mul))
		return SyntaxTree_Operator::Mul;

	if (token == toString(SyntaxTree_Operator::Div))
		return SyntaxTree_Operator::Div;

	if (token == toString(SyntaxTree_Operator::Modulo))
		return SyntaxTree_Operator::Modulo;

	if (token == toString(SyntaxTree_Operator::Pow))
		return SyntaxTree_Operator::Pow;

	if (token == toString(SyntaxTree_Operator::Root))
		return SyntaxTree_Operator::Root;

	if (token == toString(SyntaxTree_Operator::Log))
		return SyntaxTree_Operator::Log;


	if (token == toString(SyntaxTree_Operator::BitWise_OR))
		return SyntaxTree_Operator::BitWise_OR;

	if (token == toString(SyntaxTree_Operator::BitWise_AND))
		return SyntaxTree_Operator::BitWise_AND;

	if (token == toString(SyntaxTree_Operator::BitWise_XOR))
		return SyntaxTree_Operator::BitWise_XOR;

	if (token == toString(SyntaxTree_Operator::Logical_OR))
		return SyntaxTree_Operator::Logical_OR;

	if (token == toString(SyntaxTree_Operator::Logical_AND))
		return SyntaxTree_Operator::Logical_AND;

	return SyntaxTree_Operator::Invalid;
}

std::string toString(SyntaxTree_Operator op)
{
	switch (op)
	{
	case SyntaxTree_Operator::Not:
		return "!";
	case SyntaxTree_Operator::Equals:
		return "==";
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

	case SyntaxTree_Operator::Add:
		return "+";
	case SyntaxTree_Operator::Sub:
		return "-";
	case SyntaxTree_Operator::Mul:
		return "*";
	case SyntaxTree_Operator::Div:
		return "/";
	case SyntaxTree_Operator::Modulo:
		return "%";
	case SyntaxTree_Operator::Pow:
		return "**";
	case SyntaxTree_Operator::Root:
		return "</";
	case SyntaxTree_Operator::Log:
		return "log";

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

	default:
		return "<invalid>";
	}
}