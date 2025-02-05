#include "getFunctionDeclaration.h"
#include "soulChecker.h"
#include "storeArguments.h"
#include "CurrentContext.h"

using namespace std;

Result<FuncDeclaration> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData, bool isForwardDeclared, ClassInfo* currentClass)
{
	FuncDeclaration funcInfo;
	string& token = iterator.currentToken;
	while(iterator.nextToken())
	{
		if(!checkName(token))
			return ErrorInfo("function name is illigal, name: " + token, iterator.currentLine);
		
		ClassArgumentInfo* isInClass = nullptr;
		if (currentClass != nullptr)
		{
			bool isCtor = (token == "ctor");
			funcInfo.functionName = currentClass->name + "#" + token;
			isInClass = new ClassArgumentInfo(*currentClass, isCtor);
		}
		else
		{
			funcInfo.functionName = token;
		}

		if (!iterator.nextToken())
			break;

		if(token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);
		
		auto emptyVec = vector<Nesting>();
		emptyVec.emplace_back();
		CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(emptyVec)));

		RawType returnType;
		Result<StoreArguments_Result> result = storeArguments(/*out*/iterator, /*out*/metaData, context, /*out*/returnType, isInClass);
		if (result.hasError)
			return result.error;

		if (isInClass != nullptr)
			delete isInClass;

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
		if (metaData.tryGetFunction(funcInfo.functionName, context, funcInfo.args, result.value().optionals, func, iterator.currentLine, error, &funcInfoIndex))
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

		if (funcInfo.functionName == "main")
		{
			if (funcInfo.args.empty())
				return funcInfo;

			RawType& arg = funcInfo.args[0].valueType;
			if (funcInfo.args.size() > 1 && (!arg.isArray() || arg.toPrimitiveType() != PrimitiveType::str) )
				return ErrorInfo("func main only allows 'main()' or 'main(str[])' as arguments", iterator.currentLine);
		}

		return funcInfo;
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}