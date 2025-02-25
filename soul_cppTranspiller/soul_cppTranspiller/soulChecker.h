#pragma once
#include <string>
#include "Type.h"
#include "MetaData.h"
#include "CurrentContext.h"

static const std::initializer_list<char> charNumbers = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static const std::initializer_list<char> illigalNameSymbols = { '!', '@', '#', '$', '%', '^', '&', '(', ')', '-', '=', '+', '[', ']', '{', '}', '\\', '|', '~', '`', '\'', '\"', ';', ':', '<', '>', '?', '/', ',', '.' };
static const std::initializer_list<const char*> illigalNames = {"func", "out", "mut", "pass", "null", "copy", "log"};

bool checkName(const std::string& name);

bool checkValue(const std::string& value, DuckType type);
bool checkValue(const std::string& value, PrimitiveType type);
bool checkValue(const std::string& value, TypeCategory category);
bool checkValue(const std::string& value, const RawType& type, MetaData& metaData);

DuckType getDuckType_fromValue(const std::string& value);
PrimitiveType getPrimitiveType_fromValue(const std::string& value);

bool isExpression_CompileConstant(std::shared_ptr<SuperExpression>& expression, MetaData& metaData, CurrentContext& context);
