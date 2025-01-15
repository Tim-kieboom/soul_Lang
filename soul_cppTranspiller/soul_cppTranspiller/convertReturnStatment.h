#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "CurrentContext.h"
#include "ReturnStatment.h"

Result<BodyStatment_Result<ReturnStatment>> convertReturnStatment(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& returnType);
