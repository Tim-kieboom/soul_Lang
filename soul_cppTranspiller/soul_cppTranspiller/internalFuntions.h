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
	false,
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
	false,
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
	false,
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
	false,
	false
);

static const FuncDeclaration interalFunc_println_empty = FuncDeclaration
(
	"println",
	toString(PrimitiveType::none),
	{},
	std::map<std::string, TemplateType>(),
	false,
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
	false,
	true
);

static const FuncDeclaration interalFunc_Fail = FuncDeclaration
(
	"Fail",
	toString(PrimitiveType::none),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), true), "msg", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
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
	false,
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
	false,
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
	false,
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
	false,
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
	false,
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
	false,
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
	false,
	true
);

static const FuncDeclaration interalFunc_length_str = FuncDeclaration
(
	"len",
	toString(PrimitiveType::u64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::str), false), "array", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true
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
	false,
	true
);

static const FuncDeclaration interalFunc_log10 = FuncDeclaration
(
	"log",
	toString(PrimitiveType::f64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::f64), false), "base", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true
);

static const FuncDeclaration interalFunc_cast_u8 = FuncDeclaration
(
	"u8",
	toString(PrimitiveType::u8),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_u16 = FuncDeclaration
(
	"u16",
	toString(PrimitiveType::u16),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_u32 = FuncDeclaration
(
	"u32",
	toString(PrimitiveType::u32),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_u64 = FuncDeclaration
(
	"u64",
	toString(PrimitiveType::u64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_uSize = FuncDeclaration
(
	"uSize",
	toString(PrimitiveType::uSize),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_i8 = FuncDeclaration
(
	"i8",
	toString(PrimitiveType::i8),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_i16 = FuncDeclaration
(
	"i16",
	toString(PrimitiveType::i16),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_i32 = FuncDeclaration
(
	"i32",
	toString(PrimitiveType::i32),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true
);

static const FuncDeclaration interalFunc_cast_i64 = FuncDeclaration
(
	"i64",
	toString(PrimitiveType::i64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_f32 = FuncDeclaration
(
	"f32",
	toString(PrimitiveType::f32),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_f64 = FuncDeclaration
(
	"f64",
	toString(PrimitiveType::f64),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_char = FuncDeclaration
(
	"char",
	toString(PrimitiveType::char_),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_bool = FuncDeclaration
(
	"bool",
	toString(PrimitiveType::bool_),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_char_ = FuncDeclaration
(
	"char_",
	toString(PrimitiveType::char_),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
);

static const FuncDeclaration interalFunc_cast_bool_ = FuncDeclaration
(
	"bool_",
	toString(PrimitiveType::bool_),
	{
		ArgumentInfo(/*isOptional:*/false, RawType(toString(PrimitiveType::compile_dynamic_withoutStr), false), "value", ArgumentType::default_, 1)
	},
	std::map<std::string, TemplateType>(),
	false,
	true,
	/*isConstexpr*/true
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
	interalFunc__soul_format_string__,

	interalFunc_cast_u8,
	interalFunc_cast_u16,
	interalFunc_cast_u32,
	interalFunc_cast_u64,
	interalFunc_cast_uSize,
	interalFunc_cast_i8,
	interalFunc_cast_i16,
	interalFunc_cast_i32,
	interalFunc_cast_i64,
	interalFunc_cast_f32,
	interalFunc_cast_f64,
	interalFunc_cast_char,
	interalFunc_cast_bool,
	interalFunc_cast_char_,
	interalFunc_cast_bool_
};