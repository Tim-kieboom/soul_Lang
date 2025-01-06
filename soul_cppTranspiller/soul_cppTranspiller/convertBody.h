#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "FuncNode.h"
#include "CurrentContext.h"

Result<FuncNode> convertBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);
