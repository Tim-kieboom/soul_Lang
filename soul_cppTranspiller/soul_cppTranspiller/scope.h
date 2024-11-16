#pragma once
#include <string>

#include "Result.h"
#include "VarInfo.h"
#include "FuncInfo.h"

Result<VarInfo> getVarFromScope(const FuncInfo& funcInfo, const std::string& name);