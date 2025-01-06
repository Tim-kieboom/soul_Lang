#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "SyntaxTree.h"

Result<SyntaxTree> getAbstractSyntaxTree(TokenIterator&& iterator, MetaData& metaData);
