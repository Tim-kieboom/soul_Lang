#pragma once
#include "Result.h"
#include "MetaData.h"
#include "ClassNode.h"

Result<ClassNode> convertClass(TokenIterator& iterator, MetaData& metaData);
