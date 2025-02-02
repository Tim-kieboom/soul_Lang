#pragma once
#include "MetaData.h"
#include "Assignment.h"
#include "CurrentContext.h"

Result<std::string> convertAssignment_ToCpp(std::shared_ptr<SuperStatement> assignment, MetaData& metaData, CurrentContext& context);
