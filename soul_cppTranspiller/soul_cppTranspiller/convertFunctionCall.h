#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "FunctionCall.h"
#include "CurrentContext.h"

Result<std::shared_ptr<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName);
Result<std::shared_ptr<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName, RawType& shouldBeType);