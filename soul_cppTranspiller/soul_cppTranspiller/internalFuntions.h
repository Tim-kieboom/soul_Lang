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
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_println = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_print_arr = FuncDeclaration
(
	"print",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true, {TypeWrapper::array_}), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_println_arr = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true, {TypeWrapper::array_}), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_println_empty = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc__soul_format_string__ = FuncDeclaration
(
	"__soul_format_string__",
	toString(PrimitiveType::str),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "args", ArgumentType::default_, 1, /*canBeMultiple:*/true)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_Fail = FuncDeclaration
(
	"Fail",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_Throw = FuncDeclaration
(
	"Throw",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
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
	std::map<std::string, TemplateType>(),
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
	std::map<std::string, TemplateType>(),
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
	std::map<std::string, TemplateType>(),
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
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_toStr = FuncDeclaration
(
	"toStr",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), true), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_length_array = FuncDeclaration
(
	"len",
	toString(PrimitiveType::u64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic), false, {TypeWrapper::array_}), "array", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_length_str = FuncDeclaration
(
	"len",
	toString(PrimitiveType::u64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), false), "array", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_log = FuncDeclaration
(
	"log",
	toString(PrimitiveType::f64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), false), "base", ArgumentType::default_, 1),
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), false), "x", ArgumentType::default_, 2)
	},
	std::map<std::string, TemplateType>(),
	false
);

static const FuncDeclaration interalFunc_log10 = FuncDeclaration
(
	"log",
	toString(PrimitiveType::f64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::f64), false), "base", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false
);


static const std::initializer_list<FuncDeclaration> internalFunctions =
{
	interalFunc_log,
	interalFunc_Fail,
	interalFunc_Throw,
	interalFunc_print,
	interalFunc_toStr,
	interalFunc_log10,
	interalFunc_println,
	interalFunc_print_arr,
	interalFunc_checkFail,
	interalFunc_checkThrow,
	interalFunc_length_str,
	interalFunc_println_arr,
	interalFunc_length_array,
	interalFunc_println_empty,
	interalFunc_checkFail_debug,
	interalFunc_checkThrow_debug,
	interalFunc__soul_format_string__
};