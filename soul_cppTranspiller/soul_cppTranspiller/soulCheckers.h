#pragma once
#include <string>
#include <initializer_list>
#include "Type.h"

static const std::initializer_list<char> charNumbers = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static const std::initializer_list<char> illigalNameSymbols = { '!', '@', '#', '$', '%', '^', '&', '(', ')', '-', '=', '+', '[', ']', '{', '}', '\\', '|', '~', '`', '\'', '\"', ';', ':', '<', '>', '?', '/', ',', '.' };
static const std::initializer_list<const char*> illigalNames = 
{ 
	"func", "out", "mut", "pass", "empty",
	toString(Type::str).c_str(), toString(Type::i8).c_str(),
	toString(Type::f32).c_str(), toString(Type::i16).c_str(),
	toString(Type::i32).c_str(), toString(Type::i64).c_str(),
	toString(Type::ui8).c_str(), toString(Type::ui64).c_str(),
	toString(Type::ui16).c_str(), toString(Type::ui32).c_str(),
	toString(Type::c_str).c_str(), toString(Type::f64).c_str(), 
	toString(Type::bool_).c_str(), toString(Type::char_).c_str()
};

bool checkName(const std::string& name);

bool checkValue(const std::string& value, Type type);
bool checkValue(const std::string& value, DuckType type);
bool checkValue(const std::string& value, TypeCategory category);

DuckType getDuckType_fromValue(const std::string& value);
