#pragma once
#include <string>

#include "metaData.hpp"

static const FuncInfo interalFunc_print = FuncInfo
(
	"print",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::compile_dynamic), "msg", 1)
	}
);

static const FuncInfo interalFunc_println = FuncInfo
(
	"println",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::compile_dynamic), "msg", 1)
	}
);

static const FuncInfo interalFunc__soul_format_string__ = FuncInfo
(
	"__soul_format_string__",
	TypeInfo(PrimitiveType::str),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::compile_dynamic), "args", 1, /*canBeMultiple:*/true)
	}
);

static const FuncInfo interalFunc_assert = FuncInfo
(
	"assertFail",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::bool_), "condition", 1),
		ArgumentInfo(ArgumentType::tk_default,TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_assertThrow = FuncInfo
(
	"assertThrow",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, PrimitiveType::bool_, "condition", 1),
		ArgumentInfo(ArgumentType::tk_default, PrimitiveType::str, "msg", 2)
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