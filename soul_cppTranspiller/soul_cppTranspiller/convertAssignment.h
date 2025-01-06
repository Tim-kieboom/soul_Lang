#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "Assignment.h"
#include "CurrentContext.h"

Result<Assignment> convertAssignment(TokenIterator& iterator, MetaData& metaData, VarInfo& varInfo, CurrentContext& context);
