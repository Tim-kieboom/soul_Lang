#include "scope.h"

Result<VarInfo> getVarFromScope(const std::vector<Nesting>& currentScope, const MetaData& metaData, const std::string& name)
{
	//for (const Nesting& nesting : currentScope)
	//{
	//	//for (auto& var : nesting.vars)
	//	//{
	//	//	if (name == var.name)
	//	//		return var;
	//	//}
	//}

	return ErrorInfo("no varFound", 0);
}

Result<VarInfo> getVarFromScope(const FuncInfo& funcInfo, const MetaData& metaData, const std::string& name)
{
	return getVarFromScope(funcInfo.scope, metaData, name);
}