#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "stringTools.h"
#include "CurrentContext.h"
#include "BinairyExpression.h"

//'RawType* isType' is if you want to know what type the expression is
Result<std::shared_ptr<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, std::initializer_list<const char*> endTokens, RawType* isType = nullptr);

//'RawType* isType' is if you want to know what type the expression is
Result<std::shared_ptr<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& shouldBeType, std::initializer_list<const char*> endTokens, RawType* isType = nullptr);