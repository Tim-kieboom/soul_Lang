#pragma once
#include <vector>

#include "Type.h"
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "ArgumentInfo.h"


Result<std::vector<ArgumentInfo>> storeArguments(TokenIterator& iterator, MetaData& metaData, RawType& returnType);
