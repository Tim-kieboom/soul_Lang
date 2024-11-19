#pragma once
#include <sstream>
#include <string>

#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertVarSetter(TokenIterator& iterator, MetaData& metaData, const Type& type, FuncInfo& funcInfo);
