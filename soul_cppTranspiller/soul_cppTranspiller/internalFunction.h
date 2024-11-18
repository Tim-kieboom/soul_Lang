#pragma once
#include <string>

#include "Result.h"
#include "FuncInfo.h"
#include "metaData.hpp"
#include "TokenIterator.h"

static const FuncInfo interalFunc_print = FuncInfo
(
	"print", 
	{ 
		ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 1)
	}
);

static const FuncInfo interalFunc_println = FuncInfo
(
	"print", 
	{ 
		ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 1)
	}
);

static const FuncInfo interalFunc__soul_format_string__ = FuncInfo
(
	"__soul_format_string__", 
	{ 
		ArgumentInfo(ArgumentType::tk_default, Type::compile_dynamic, "args", 1, /*canBeMultiple:*/true)
	}
);

static const FuncInfo interalFunc_assert = FuncInfo
(
	"assertFail", 
	{ 
		ArgumentInfo(ArgumentType::tk_default, Type::bool_, "condition", 1), 
		ArgumentInfo(ArgumentType::tk_default, Type::str, "msg", 2) 
	}
);

static const FuncInfo interalFunc_assertThrow = FuncInfo
(
	"assertThrow", 
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