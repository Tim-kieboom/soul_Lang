#pragma once
#include <string>

#include "MetaData.hpp"

Result<VarInfo> getVarFromScope(const FuncInfo& funcInfo, const MetaData& metaData, const std::string& name);
Result<VarInfo> getVarFromScope(const std::vector<VarInfo>& currentScope, const MetaData& metaData, const std::string& name);