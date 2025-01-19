#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "WhileStatment.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<SuperConditionalStatment>> convertWhileStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);
