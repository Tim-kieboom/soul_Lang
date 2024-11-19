#pragma once
#include <string>

#include "Result.h"
#include "FuncInfo.h"
#include "metaData.hpp"
#include "TokenIterator.h"

static const FuncInfo interalFunc_print = FuncInfo
(
	"print", 
	Type::none,
	{ 
		ArgumentInfo(ArgumentType::tk_default, Type::compile_dynamic, "msg", 1)
	}
);

static const FuncInfo interalFunc_println = FuncInfo
(
	"println", 
	Type::none,
	{
		ArgumentInfo(ArgumentType::tk_default, Type::compile_dynamic, "msg", 1)
	}
);

static const FuncInfo interalFunc__soul_format_string__ = FuncInfo
(
	"__soul_format_string__", 
	Type::str,
	{
		ArgumentInfo(ArgumentType::tk_default, Type::compile_dynamic, "args", 1, /*canBeMultiple:*/true)
	}
);

static const FuncInfo interalFunc_assert = FuncInfo
(
	"assertFail", 
	Type::none,
	{
		ArgumentInfo(ArgumentType::tk_default, Type::bool_, "condition", 1), 
		ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 2) 
	}
);

static const FuncInfo interalFunc_assertThrow = FuncInfo
(
	"assertThrow", 
	Type::none,
	{
		ArgumentInfo(ArgumentType::tk_default, Type::bool_, "condition", 1), 
		ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 2) 
	}
);

static const std::initializer_list<FuncInfo> internalFunctions =
{
	interalFunc_print,
	interalFunc_assert,
	interalFunc_println,
	interalFunc_assertThrow,
	interalFunc__soul_format_string__
};