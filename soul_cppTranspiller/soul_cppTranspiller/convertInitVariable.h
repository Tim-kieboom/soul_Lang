#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "InitVariable.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, RawType& type);
Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, bool isConst);
Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context);
Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, bool isConst);
