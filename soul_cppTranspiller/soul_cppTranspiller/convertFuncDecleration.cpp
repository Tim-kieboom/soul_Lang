#include "convertFuncDecleration.h"
#include "soulCheckers.h"
#include "Nesting.h"
#include "ArgumentInfo.h"
#include "storeArguments.h"
#include "cppConverter.h"

using namespace std;
static inline Result<string> convertFuncDeclaration_arguments
(
	/*out*/TokenIterator& iterator,
	/*out*/MetaData& metaData,
	/*out*/FuncInfo& funcInfo,
	/*out*/vector<Nesting>& scope
)
{
	Result<void> result = storeArguments(/*out*/iterator, /*out*/metaData, /*out*/funcInfo, /*out*/scope.at(0));
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

		if (metaData.TryGetfuncInfo(token, _))
			return ErrorInfo("function already exsists, name: " + token, iterator.currentLine);

		funcInfo = FuncInfo(token);
		vector<Nesting>& scope = funcInfo.scope;

		if (!iterator.nextToken())
			break;

		if(token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);
	
		Result<string> argsResult = convertFuncDeclaration_arguments(iterator, metaData, funcInfo, scope);
		if (argsResult.hasError)
			return argsResult.error;

		string returnType = typeToCppType(funcInfo.returnType);

		metaData.addFuncInfo(funcInfo.funcName, funcInfo);
		ss << returnType << ' ' << funcInfo.funcName << ' ' << argsResult.value();
		if (metaData.transpillerOption.addEndLines)
			ss << '\n';

		return ss.str();
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}
