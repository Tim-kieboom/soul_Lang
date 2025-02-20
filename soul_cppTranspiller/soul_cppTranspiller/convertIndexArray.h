#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "IndexArray.h"
#include "CurrentContext.h"
#include "SuperStatement.h"

Result<BodyStatment_Result<IndexArray>> convertIndexArray(TokenIterator& iterator, std::shared_ptr<SuperExpression>& arrayExpression, MetaData& metaData, CurrentContext& context);
