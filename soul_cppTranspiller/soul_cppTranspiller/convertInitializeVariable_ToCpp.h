#pragma once
#include "MetaData.h"
#include "InitVariable.h"
#include "CurrentContext.h"

Result<std::string> convertInitializeVariable_ToCpp(std::shared_ptr<SuperStatement> node, MetaData& metaData, CurrentContext& context);
