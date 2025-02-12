#pragma once
#include "MetaData.h"
#include "InitVariable.h"
#include "CurrentContext.h"

Result<std::string> convertCompileConstVariable_ToCpp(std::shared_ptr<SuperStatement> node, MetaData& metaData, CurrentContext& context);
