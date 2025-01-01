#include "convertFuncDecleration.h"
#include "soulCheckers.h"
#include "Nesting.h"
#include "ArgumentInfo.h"
#include "storeArguments.h"
#include "cppConverter.h"

using namespace std;

static inline string toString(vector<ArgumentInfo>& args)
{
	stringstream ss;
	ss << '[';
	for (const ArgumentInfo& arg : args)
		ss << toString(arg);
	ss << ']';
	return ss.str();
}

static inline Result<string> convertFuncDeclaration_arguments
(
	/*out*/TokenIterator& iterator,
	/*out*/MetaData& metaData,
	/*out*/FuncInfo& funcInfo,
	/*out*/vector<Nesting>& scope
)
{
	Result<void> result = storeArguments(/*out*/iterator, /*out*/metaData, /*out*/funcInfo, /*out*/funcInfo.scope.at(0));
	if (result.hasError)
		return result.error;

	vector<ArgumentInfo>& args = funcInfo.args;
	stringstream ss;

	ss << '(';
	for (uint32_t i = 0; i < args.size(); i++)
	{
		const ArgumentInfo& arg = args.at(i);
		ss << "/*" << toString(arg.argType) << "*/" << ArgToCppArg(arg.argType, arg.valueType) << ' ' << arg.name;

		if (i != args.size() - 1)
			ss << ", ";
	}
	ss << ')';

	return ss.str();
}

Result<string> convertFuncDeclaration(/*out*/TokenIterator& iterator, /*out*/MetaData& metaData, /*out*/FuncInfo& funcInfo)
{
	stringstream ss;
	FuncInfo _;

	string& token = iterator.currentToken;
	while(iterator.nextToken())
	{
		if(!checkName(token))
			return ErrorInfo("function name is illigal, name: " + token, iterator.currentLine);

		funcInfo = FuncInfo(token);

		if (!iterator.nextToken())
			break;

		if(token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);
	
		Result<string> argsResult = convertFuncDeclaration_arguments(iterator, metaData, funcInfo, funcInfo.scope);
		if (argsResult.hasError)
			return argsResult.error;

		string returnType = typeToCppType(funcInfo.returnType);

		if (metaData.TryGetfuncInfo(token, funcInfo.args, _))
			return ErrorInfo("function with these arguments already exists, name: \'" + token + "\', args: \'" + toString(funcInfo.args) + "\'", iterator.currentLine);

		metaData.addFuncInfo(funcInfo.funcName, funcInfo);
		ss << returnType << ' ' << funcInfo.funcName << argsResult.value();
		if (metaData.transpillerOption.addEndLines)
			ss << '\n';

		return ss.str();
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}
