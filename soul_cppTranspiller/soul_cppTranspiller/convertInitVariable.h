#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "InitVariable.h"
#include "CurrentContext.h"

Result<std::vector<std::shared_ptr<SuperStatement>>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context);
