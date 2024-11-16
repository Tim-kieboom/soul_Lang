#include "scope.h"

Result<VarInfo> getVarFromScope(const FuncInfo& funcInfo, const std::string& name)
{
	for (const VarInfo& var : funcInfo.scope)
	{
		if (name == var.name)
			return var;
	}

	return ErrorInfo("no varFound", 0);
}