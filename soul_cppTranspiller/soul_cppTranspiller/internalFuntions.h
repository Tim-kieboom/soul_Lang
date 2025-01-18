#pragma once
#include <string>
#include "MetaData.h"

static const FuncDeclaration interalFunc_print = FuncDeclaration
(
	"print",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "msg", ArgumentType::default_, 1)
	},
	false
);

static const FuncDeclaration interalFunc_println = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "msg", ArgumentType::default_, 1)
	},
	false
);

static const FuncDeclaration interalFunc_println_empty = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{},
	false
);

static const FuncDeclaration interalFunc__soul_format_string__ = FuncDeclaration
(
	"__soul_format_string__",
	toString(PrimitiveType::str),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "args", ArgumentType::default_, 1, /*canBeMultiple:*/true)
	},
	false
);

static const FuncDeclaration interalFunc_Fail = FuncDeclaration
(
	"Fail",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 1)
	},
	false
);

static const FuncDeclaration interalFunc_Throw = FuncDeclaration
(
	"Throw",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 1)
	},
	false
);

static const FuncDeclaration interalFunc_checkFail_debug = FuncDeclaration
(
	"checkFail_debug",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::bool_), true), "condition", ArgumentType::default_, 1),
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 2)
	},
	false
);

static const FuncDeclaration interalFunc_checkThrow_debug = FuncDeclaration
(
	"checkThrow_debug",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::bool_), true), "condition", ArgumentType::default_, 1),
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 2)
	},
	false
);

static const FuncDeclaration interalFunc_checkFail = FuncDeclaration
(
	"checkFail",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::bool_), true), "condition", ArgumentType::default_, 1),
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 2)
	},
	false
);

static const FuncDeclaration interalFunc_checkThrow = FuncDeclaration
(
	"checkThrow",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::bool_), true), "condition", ArgumentType::default_, 1),
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 2)
	},
	false
);

static const FuncDeclaration interalFunc_toStr = FuncDeclaration
(
	"toStr",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "value", ArgumentType::default_, 1)
	},
	false
);

static const std::initializer_list<FuncDeclaration> internalFunctions =
{
	interalFunc_Fail,
	interalFunc_Throw,
	interalFunc_print,
	interalFunc_toStr,
	interalFunc_println,
	interalFunc_checkFail,
	interalFunc_checkThrow,
	interalFunc_println_empty,
	interalFunc_checkFail_debug,
	interalFunc_checkThrow_debug,
	interalFunc__soul_format_string__
};