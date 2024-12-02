#include "VarInit.h"
#include "varSetter.h"
#include "soulCheckers.h"
#include "cppConverter.h"

#define ERROR_varInit ErrorInfo("incomplete functionBody funcName: \'" + string(callFunc.funcName) + '\'', iterator.currentLine)
using namespace std;

Result<string> convertVarInit(Type type, bool isMutable, TokenIterator& iterator, MetaData& metaData, FuncInfo& callFunc, FuncInfo& funcInfo, ScopeIterator& scope)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_varInit;

	if (!checkName(token))
		return ErrorInfo("name invalid name: " + token, iterator.currentLine);

	ss << typeToCppType(type) << ' ' << token;

	auto varInfo = VarInfo(string_copyTo_c_str(token), type, isMutable);
	scope.getCurrentNesting().addVariable(varInfo);

	if (!iterator.nextToken(/*out*/token))
		return ERROR_varInit;

	if (token == ";")
	{
		ss << ';';
		return ss.str();
	}

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	ss << " = ";

	if (!iterator.nextToken(/*out*/token))
		return ERROR_varInit;

	Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo, scope, varSetter_Option::endSemiColon);
	if (varSetterResult.hasError)
		return varSetterResult.error;

	ss << varSetterResult.value();
	return ss.str();
}