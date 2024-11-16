#pragma once
#include "Type.h"
#include "ArgumentType.h"

const char* typeToCppType(const Type type);
const char* ArgToCppArg(const ArgumentType argType, const Type type);

