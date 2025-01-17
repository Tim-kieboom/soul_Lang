#include "getFunctionDeclaration.h"
#include "soulChecker.h"
#include "storeArguments.h"
#include "CurrentContext.h"

using namespace std;

Result<FuncDeclaration> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData, bool isForwardDeclared)
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

		if (funcInfo.functionName == "main")
		{
			bool isCorrectType = returnType.typeWrappers.empty() &&
								 returnType.toPrimitiveType() == PrimitiveType::i32;

			if (!isCorrectType)
				return ErrorInfo("'func main' needs to have type 'i32'", iterator.currentLine);
		}

		funcInfo.returnType = toString(returnType);
		funcInfo.args = result.value().args;
		funcInfo.pushOptionals(result.value().optionals);

		uint64_t funcInfoIndex = 0;
		ErrorInfo error;
		FuncDeclaration func;
		if (metaData.tryGetFuncInfo(funcInfo.functionName, funcInfo.args, result.value().optionals, func, iterator.currentLine, error, &funcInfoIndex))
		{
			if(!func.isForwardDeclared)
				return ErrorInfo("function with these arguments already exists, name: \'" + token + "\', args: \'" + toString(funcInfo.args) + "\'\n" + error.message, iterator.currentLine);
		}

		if (isForwardDeclared && funcInfo.functionName != "main")
		{
			vector<FuncDeclaration>& funcs = metaData.funcStore[funcInfo.functionName];
			FuncDeclaration& funcRef = funcs.at(funcInfoIndex);
			funcRef.isForwardDeclared = false;
		}
		else
		{
			metaData.addFunction(funcInfo.functionName, funcInfo);
		}

		return funcInfo;
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}