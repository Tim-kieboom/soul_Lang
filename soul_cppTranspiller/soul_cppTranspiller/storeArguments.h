#pragma once
#include "Result.h"
#include "ArgumentType.h"
#include "TokenIterator.h"

Result<void*> storeArguments(/*out*/ TokenIterator& iterator, /*out*/ MetaData& metaData, /*out*/ FuncInfo& funcInfo, /*out*/ Nesting& scope);
