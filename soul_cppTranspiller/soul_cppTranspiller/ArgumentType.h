#pragma once
#include <string>

enum class ArgumentType
{
	invalid,

	tk_default,
	tk_mut,
	tk_out,

	tk_optional,
	tk_optional_mut,
	tk_optional_out
};

const char* toString(ArgumentType argType);

bool argType_isMutable(ArgumentType type);
bool argType_isOptions(ArgumentType type);
ArgumentType getArgType_symbol(std::string& str);
ArgumentType argType_ChangeToOptional(ArgumentType oldType);