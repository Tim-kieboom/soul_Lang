#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "CurrentContext.h"
#include "SuperConditionalStatment.h"

Result<BodyStatment_Result<SuperConditionalStatment>> convertForStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);
