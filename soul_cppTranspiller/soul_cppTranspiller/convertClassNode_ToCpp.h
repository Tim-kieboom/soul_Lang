#pragma once
#include "MetaData.h"
#include "ClassNode.h"

Result<std::string> convertClassNode_ToCpp(std::shared_ptr<ClassNode> classNode, MetaData& metaData);
