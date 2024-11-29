#pragma once
#include <string>
#include <vector>

#include "Result.h"
#include "TokenIterator.h"

Result<std::string> convertFunctionBody(TokenIterator& iterator, /*out*/ FuncInfo& funcInfo, MetaData& metaData, Nesting& currentNesting);

