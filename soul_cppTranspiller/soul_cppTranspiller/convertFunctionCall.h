#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "FunctionCall.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName);
Result<BodyStatment_Result<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName, RawType& shouldBeType);