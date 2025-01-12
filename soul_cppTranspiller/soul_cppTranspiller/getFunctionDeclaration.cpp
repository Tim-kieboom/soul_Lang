#include "getFunctionDeclaration.h"
#include "soulChecker.h"
#include "storeArguments.h"
#include "CurrentContext.h"

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
		
		auto emptyVec = vector<Nesting>();
		emptyVec.emplace_back();
		CurrentContext context = CurrentContext(ScopeIterator(emptyVec));

		RawType returnType;
		Result<StoreArguments_Result> result = storeArguments(/*out*/iterator, /*out*/metaData, context, /*out*/returnType);
		if (result.hasError)
			return result.error;

		funcInfo.returnType = toString(returnType);
		funcInfo.args = result.value().args;
		funcInfo.pushOptionals(result.value().optionals);

		FuncDeclaration _;
		if (metaData.tryGetFuncInfo(funcInfo.functionName, funcInfo.args, result.value().optionals, _))
			return ErrorInfo("function with these arguments already exists, name: \'" + token + "\', args: \'" + toString(funcInfo.args) + "\'", iterator.currentLine);

		metaData.addFunction(funcInfo.functionName, funcInfo);
		return funcInfo;
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}