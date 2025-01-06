#include "getFunctionDeclaration.h"
#include "soulChecker.h"
#include "storeArguments.h"

using namespace std;

Result<FuncDeclaration> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData)
{
	FuncDeclaration funcInfo;
	string& token = iterator.currentToken;
	while(iterator.nextToken())
	{
		if(!checkName(token))
			return ErrorInfo("function name is illigal, name: " + token, iterator.currentLine);

		funcInfo.functionName = token;

		if (!iterator.nextToken())
			break;

		if(token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);
		
		RawType returnType;
		Result<vector<ArgumentInfo>> result = storeArguments(/*out*/iterator, /*out*/metaData, /*out*/returnType);
		if (result.hasError)
			return result.error;

		funcInfo.returnType = toString(returnType);
		funcInfo.args = result.value();

		FuncDeclaration _;
		if (metaData.tryGetFuncInfo(funcInfo.functionName, funcInfo.args, _))
			return ErrorInfo("function with these arguments already exists, name: \'" + token + "\', args: \'" + toString(funcInfo.args) + "\'", iterator.currentLine);

		metaData.addFunction(funcInfo.functionName, funcInfo);
		return funcInfo;
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}