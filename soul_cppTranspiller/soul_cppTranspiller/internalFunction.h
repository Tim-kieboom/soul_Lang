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

static const FuncInfo interalFunc_Fail = FuncInfo
(
	"Fail",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default,TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_Throw = FuncInfo
(
	"Throw",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_checkFail_debug = FuncInfo
(
	"checkFail_debug",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::bool_), "condition", 1),
		ArgumentInfo(ArgumentType::tk_default,TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_checkThrow_debug = FuncInfo
(
	"checkThrow_debug",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::bool_), "condition", 1),
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_checkFail = FuncInfo
(
	"checkFail",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::bool_), "condition", 1),
		ArgumentInfo(ArgumentType::tk_default,TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_checkThrow = FuncInfo
(
	"checkThrow",
	TypeInfo(PrimitiveType::none),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::bool_), "condition", 1),
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::str), "msg", 2)
	}
);

static const FuncInfo interalFunc_toStr = FuncInfo
(
	"toStr",
	TypeInfo(PrimitiveType::str),
	{
		ArgumentInfo(ArgumentType::tk_default, TypeInfo(PrimitiveType::compile_dynamic), "value", 1)
	}
);

static const std::initializer_list<FuncInfo> internalFunctions =
{
	interalFunc_Fail,
	interalFunc_Throw,
	interalFunc_print,
	interalFunc_toStr,
	interalFunc_println,
	interalFunc_checkFail,
	interalFunc_checkThrow,
	interalFunc_checkFail_debug,
	interalFunc_checkThrow_debug,
	interalFunc__soul_format_string__
};