#pragma once
#include "MetaData.hpp"
#include "ArgumentType.h"

std::string typeToCppType(const TypeInfo& type);
std::string ArgToCppArg(const ArgumentType argType, const TypeInfo& type);