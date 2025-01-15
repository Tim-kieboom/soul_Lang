#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "InitVariable.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context);
