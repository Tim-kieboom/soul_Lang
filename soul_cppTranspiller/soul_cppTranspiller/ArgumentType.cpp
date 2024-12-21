#include "ArgumentType.h"

const char* toString(ArgumentType argType)
{
	switch (argType)
	{
	case ArgumentType::tk_default:
		return "default";
	case ArgumentType::tk_mut:
		return "mut";
	case ArgumentType::tk_out:
		return "out";
	case ArgumentType::tk_optional:
		return "optional";
	case ArgumentType::tk_optional_mut:
		return "optional_mut";
	case ArgumentType::tk_optional_out:
		return "optional_out";

	default:
	case ArgumentType::invalid:
		return "invalid";
	}
}

bool argType_isOut(ArgumentType type)
{
	return type == ArgumentType::tk_out || type == ArgumentType::tk_optional_out;
}

bool argType_isMutable(ArgumentType type)
{
	return !(type == ArgumentType::tk_default || type == ArgumentType::tk_optional);
}

bool argType_isOptions(ArgumentType type)
{
	return type == ArgumentType::tk_optional || type == ArgumentType::tk_optional_mut || type == ArgumentType::tk_optional_out;
}

ArgumentType getArgType_symbol(std::string& str)
{
	if (str == "out")
		return ArgumentType::tk_out;

	if (str == "mut")
		return ArgumentType::tk_mut;

	return ArgumentType::invalid;
}

ArgumentType argType_ChangeToOptional(ArgumentType oldType)
{
	switch (oldType)
	{
	case ArgumentType::tk_default:
		return ArgumentType::tk_optional;

	case ArgumentType::tk_mut:
		return ArgumentType::tk_optional_mut;

	case ArgumentType::tk_out:
		return ArgumentType::tk_optional_out;

	default:
		return ArgumentType::invalid;
	}
}