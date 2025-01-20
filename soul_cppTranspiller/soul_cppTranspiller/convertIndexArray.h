#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "IndexArray.h"
#include "CurrentContext.h"
#include "SuperStatement.h"

Result<BodyStatment_Result<IndexArray>> convertIndexArray(TokenIterator& iterator, MetaData& metaData, CurrentContext& context);
