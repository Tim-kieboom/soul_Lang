#pragma once
#include "MetaData.h"
#include "CurrentContext.h"
#include "SuperExpression.h"

Result<std::string> SuperExpression_ToCpp(std::shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context);