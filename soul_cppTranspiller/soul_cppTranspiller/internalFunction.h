#pragma once
#include "FuncInfo.h"

static const FuncInfo interalFunc_assert = FuncInfo("assertFail", { ArgumentInfo(ArgumentType::tk_default, Type::bool_, "condition", 1), ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 2) });
static const FuncInfo interalFunc_assertThrow = FuncInfo("assertThrow", { ArgumentInfo(ArgumentType::tk_default, Type::bool_, "condition", 1), ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 2) });

static const std::initializer_list<FuncInfo> internalFunctions =
{
	interalFunc_assert,
	interalFunc_assertThrow
};