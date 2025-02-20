#pragma once
#include "Token.h"
#include "MetaData.h"
#include "BodyNode.h"
#include "MemberExpression.h"

Result<BodyStatment_Result<MemberExpression>> convertMemberExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, VarInfo& parentVar, bool shouldBeMultable, RawType& memberType);
