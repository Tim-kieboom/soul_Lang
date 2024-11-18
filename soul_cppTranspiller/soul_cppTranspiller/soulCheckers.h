#pragma once
#include <string>
#include <initializer_list>
#include "Type.h"

static const std::initializer_list<char> charNumbers = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static const std::initializer_list<char> illigalNameSymbols = { '!', '@', '#', '$', '%', '^', '&', '(', ')', '-', '=', '+', '[', ']', '{', '}', '\\', '|', '~', '`', '\'', '\"', ';', ':', '<', '>', '?', '/', ',', '.' };
static const std::initializer_list<const char*> illigalNames = { "func" };

bool checkName(const std::string& name);

bool checkValue(const std::string& value, Type type);
bool checkValue(const std::string& value, DuckType type);
bool checkValue(const std::string& value, TypeCategory category);

