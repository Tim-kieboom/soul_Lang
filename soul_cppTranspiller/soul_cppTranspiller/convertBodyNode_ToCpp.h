#pragma once
#include "MetaData.h"
#include "BodyNode.h"
#include "CurrentContext.h"
#include "SuperMainNodes.h"

Result<std::string> convertBodyNode_ToCpp(std::shared_ptr<BodyNode> body, MetaData& metaData);

