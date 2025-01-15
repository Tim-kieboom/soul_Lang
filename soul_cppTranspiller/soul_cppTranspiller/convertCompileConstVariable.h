#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "CurrentContext.h"
#include "CompileConstVariable.h"

Result<BodyStatment_Result<CompileConstVariable>> convertCompileConstVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context);
