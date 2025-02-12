#pragma once
#include "MetaData.h"
#include "SyntaxTree.h"

Result<std::string> convertToCpp(SyntaxTree& tree, MetaData& metaData);
