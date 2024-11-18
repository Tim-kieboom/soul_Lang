#include "funcDecleration.h"
#include <sstream>

#include "soulCheckers.h"
#include "ArgumentType.h"
#include "cppConverter.h"
#include "storeArguments.h"

using namespace std;

static inline Result<string> convertFuncDeclaration_arguments
	(
		/*out*/TokenIterator& iterator, 
		/*out*/MetaData& metaData,
		/*out*/FuncInfo& funcInfo, 
		/*out*/vector<VarInfo>& scope
	)
{
	Result<void*> result = storeArguments(/*out*/iterator, /*out*/metaData, /*out*/funcInfo, /*out*/scope);
	if (result.hasError)
		return result.error;

	vector<ArgumentInfo>& args = funcInfo.args;
	stringstream ss;

	ss << '(';
	for (uint32_t i = 0; i < args.size(); i++)
	{
		const ArgumentInfo& arg = args.at(i);
		ss << "/*" << toString(arg.argType) << "*/ " << ArgToCppArg(arg.argType, arg.valueType) << ' ' << arg.name;

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

	string token;
	while(iterator.nextToken(/*out*/token))
	{
		if (!checkName(token))
			return ErrorInfo("function name is illigal, name: " + token, iterator.currentLine);

		if (metaData.TryGetfuncInfo(token, _))
			return ErrorInfo("function already exsists, name: " + token, iterator.currentLine);
	
		funcInfo = FuncInfo(string_copyTo_c_str(token));

		vector<VarInfo>& scope = funcInfo.scope;
		scope.reserve(20);

		if (!iterator.nextToken(/*out*/token))
			break;

		if (token != "(")
			return ErrorInfo("function Declaration missing '(' ", iterator.currentLine);

		Result<string> argsResult = convertFuncDeclaration_arguments(iterator, metaData, funcInfo, scope);
		if (argsResult.hasError)
			return argsResult.error;

		string returnType = typeToCppType(funcInfo.returnType);

		metaData.addFuncInfo(funcInfo.funcName, funcInfo);
		ss << returnType << ' ' << funcInfo.funcName << ' ' << argsResult.value() << '\n';

		return ss.str();
	}

	return ErrorInfo("function declaration incomplete", iterator.currentLine);
}
