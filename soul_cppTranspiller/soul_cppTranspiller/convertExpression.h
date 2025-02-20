#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "stringTools.h"
#include "CurrentContext.h"
#include "BinairyExpression.h"
#include "SuperStatement.h"

Result<BodyStatment_Result<SuperExpression>> getVariableExpression
(
    TokenIterator& iterator,
    MetaData& metaData,
    CurrentContext& context,
    Result<VarInfo*> varResult,
    RawType* shouldBeType,
    RawType* isType,
    bool shouldBeMutable
);

//'RawType* isType' is if you want to know what type the expression is
Result<BodyStatment_Result<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, std::initializer_list<const char*> endTokens, bool shouldBeMutable, RawType* isType = nullptr);

//'RawType* isType' is if you want to know what type the expression is
Result<BodyStatment_Result<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& shouldBeType, std::initializer_list<const char*> endTokens, bool shouldBeMutable, RawType* isType = nullptr);