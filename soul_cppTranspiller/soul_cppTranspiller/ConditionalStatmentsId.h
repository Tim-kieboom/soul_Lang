#pragma once
#include <string>

enum class ConditionalStatmentsId
{
	invalid,

	if_,
	else_,
	else_if_,
	
	for_,
	while_,

	switch_
};

std::string toString(ConditionalStatmentsId id)
{
	switch(id)
	{
		case ConditionalStatmentsId::if_:
			return "if";
		case ConditionalStatmentsId::else_:
			return "else";
		case ConditionalStatmentsId::else_if_:
			return "else if";

		case ConditionalStatmentsId::for_:
			return "for";
		case ConditionalStatmentsId::while_:
			return "while";

		case ConditionalStatmentsId::switch_:	
			return "switch";

		case ConditionalStatmentsId::invalid:
		default:
			return "<invalid";
	}
}

ConditionalStatmentsId getConditionStatment(std::string& token, std::string& nextToken)
{
	if (token == toString(ConditionalStatmentsId::else_) && nextToken == toString(ConditionalStatmentsId::if_))
		return ConditionalStatmentsId::else_if_;


	if (token == toString(ConditionalStatmentsId::if_))
		return ConditionalStatmentsId::if_;

	if (token == toString(ConditionalStatmentsId::else_))
		return ConditionalStatmentsId::else_;

	if (token == toString(ConditionalStatmentsId::for_))
		return ConditionalStatmentsId::for_;

	if (token == toString(ConditionalStatmentsId::while_))
		return ConditionalStatmentsId::while_;

	if (token == toString(ConditionalStatmentsId::switch_))
		return ConditionalStatmentsId::switch_;

	return ConditionalStatmentsId::invalid;
}