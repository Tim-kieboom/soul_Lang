#include "scope.h"

Result<VarInfo> getVarFromScope(const std::vector<VarInfo>& currentScope, const MetaData& metaData, const std::string& name)
{
	for (const VarInfo& var : currentScope)
	{
		if (name == var.name)
			return var;
	}

	for (const VarInfo& var : metaData.globalScope)
	{
		if (name == var.name)
			return var;
	}

	return ErrorInfo("no varFound", 0);
}

Result<VarInfo> getVarFromScope(const FuncInfo& funcInfo, const MetaData& metaData, const std::string& name)
{
	return getVarFromScope(funcInfo.scope, metaData, name);
}