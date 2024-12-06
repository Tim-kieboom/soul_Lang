#pragma once
#include <string>
#include <initializer_list>
#include "TypeInfo.h"

static const std::initializer_list<char> charNumbers = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static const std::initializer_list<char> illigalNameSymbols = { '!', '@', '#', '$', '%', '^', '&', '(', ')', '-', '=', '+', '[', ']', '{', '}', '\\', '|', '~', '`', '\'', '\"', ';', ':', '<', '>', '?', '/', ',', '.' };
static const std::initializer_list<const char*> illigalNames =
{
	"func", "out", "mut", "pass", "empty",
	toString(PrimitiveType::str).c_str(), toString(PrimitiveType::i8).c_str(),
	toString(PrimitiveType::f32).c_str(), toString(PrimitiveType::i16).c_str(),
	toString(PrimitiveType::i32).c_str(), toString(PrimitiveType::i64).c_str(),
	toString(PrimitiveType::ui8).c_str(), toString(PrimitiveType::ui64).c_str(),
	toString(PrimitiveType::ui16).c_str(), toString(PrimitiveType::ui32).c_str(),
	toString(PrimitiveType::c_str).c_str(), toString(PrimitiveType::f64).c_str(),
	toString(PrimitiveType::bool_).c_str(), toString(PrimitiveType::char_).c_str()
};

bool checkName(const std::string& name);

bool checkValue(const std::string& value, PrimitiveType type);
bool checkValue(const std::string& value, DuckType type);
bool checkValue(const std::string& value, TypeCategory category);

DuckType getDuckType_fromValue(const std::string& value);

