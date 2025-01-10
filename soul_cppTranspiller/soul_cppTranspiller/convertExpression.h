#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "stringTools.h"
#include "CurrentContext.h"
#include "BinairyExpression.h"

Result<std::shared_ptr<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& shouldBeType, std::initializer_list<const char*> endToken);