#pragma once
#include "convertVarSetter.h"

Result<std::string> convertVar(VarInfo& varInfo, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope, std::string* className, varSetter_Option option = varSetter_Option::endSemiColon);
