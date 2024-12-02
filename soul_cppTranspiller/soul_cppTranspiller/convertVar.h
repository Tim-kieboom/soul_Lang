#pragma once
#include "scope.h"
#include "Result.h"
#include "varSetter.h"
#include "metaData.hpp"
#include "TokenIterator.h"

Result<std::string> convertVar(VarInfo& varInfo, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope, varSetter_Option option = varSetter_Option::endSemiColon);