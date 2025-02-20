#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "Assignment.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<Assignment>> convertAssignment(TokenIterator& iterator, std::shared_ptr<SuperExpression> setVariable, RawType& setVariableType, MetaData& metaData, VarInfo* varInfo, CurrentContext& context);
