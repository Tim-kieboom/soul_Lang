#include "getFunctionDeclaration.h"
#include "soulChecker.h"
#include "storeArguments.h"
#include "CurrentContext.h"
#include "getTemplateTypes.h"

using namespace std;

static inline Result<void> checkIfArgsAreConst(TokenIterator& iterator, StoreArguments_Result& args)
{
	for (auto& arg : args.args)
	{
		if (arg.valueType.isMutable)
			return ErrorInfo("\'" + toString(arg) + "\' has to be inmutable because function is 'Functional'", iterator.currentLine);
	}

	for (auto& arg : args.optionals)
	{
		if (arg.valueType.isMutable)
			return ErrorInfo("\'" + toString(arg) + "\' has to be inmutable because function is 'Functional'", iterator.currentLine);
	}

	return {};
}

Result<FuncDeclaration_Result> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData, bool isForwardDeclared, CurrentContext& context, ClassInfo* currentClass)
{
	std::shared_ptr<TemplateTypes> templateTypes;

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

		if (context.functionRuleSet == CurrentContext::FuncRuleSet::Functional)
		{
			funcInfo.isConstexpr = true;
		}

		if (!iterator.nextToken())
			break;

		if (token == "<")
		{
			Result<std::shared_ptr<TemplateTypes>> templatesTypesResult = getTemplateTypes(iterator, context);
			if (templatesTypesResult.hasError)
				return templatesTypesResult.error;

			templateTypes = templatesTypesResult.value();
			funcInfo.templateTypes = templateTypes->templateTypes;
			if (!iterator.nextToken())
				break;
		}

		if(token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);

		RawType returnType;
		Result<StoreArguments_Result> argsResult = storeArguments(/*out*/iterator, /*out*/metaData, context, /*out*/returnType, isInClass);
		if (argsResult.hasError)
			return argsResult.error;

		if(context.functionRuleSet == CurrentContext::FuncRuleSet::Functional)
		{
			Result<void> result = checkIfArgsAreConst(iterator, argsResult.value());
			if (result.hasError)
				return result.error;
		}

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
		funcInfo.args = argsResult.value().args;
		funcInfo.pushOptionals(argsResult.value().optionals);

		uint64_t funcInfoIndex = 0;
		ErrorInfo error;
		FuncDeclaration func;
		if (metaData.tryGetFunction(funcInfo.functionName, context, funcInfo.args, argsResult.value().optionals, func, iterator.currentLine, error, &funcInfoIndex))
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
				return FuncDeclaration_Result(funcInfo, templateTypes);

			RawType& arg = funcInfo.args[0].valueType;
			if (funcInfo.args.size() > 1 && (!arg.isArray() || arg.toPrimitiveType() != PrimitiveType::str) )
				return ErrorInfo("func main only allows 'main()' or 'main(str[])' as arguments", iterator.currentLine);
		}

		return FuncDeclaration_Result(funcInfo, templateTypes);
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}