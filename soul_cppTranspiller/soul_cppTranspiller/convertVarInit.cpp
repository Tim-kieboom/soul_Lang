#include "convertVarInit.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "convertVarSetter.h"
using namespace std;

static inline ErrorInfo ERROR_convertVarInit_outOfBounds(FuncInfo& callFunc, TokenIterator& iterator)
{
	return ErrorInfo("incomplete functionBody funcName: \'" + string(callFunc.funcName) + '\'', iterator.currentLine);
}

Result<string> convertVarInit
(
	TypeInfo& type, 
	TokenIterator& iterator, 
	MetaData& metaData, 
	FuncInfo& callFunc, 
	FuncInfo& funcInfo, 
	ScopeIterator& scope
)
{
	stringstream ss;
	string& token = iterator.currentToken;

	if (!iterator.nextToken())
		return ERROR_convertVarInit_outOfBounds(callFunc, iterator);

	if (!checkName(token))
		return ErrorInfo("name invalid name: \'" + token + "\'", iterator.currentLine);

	ss << typeToCppType(type) << ' ' << token;

	VarInfo varInfo = VarInfo(token, type);
	scope.getCurrentNesting().addVariable(varInfo);

	if (!iterator.nextToken())
		return ERROR_convertVarInit_outOfBounds(callFunc, iterator);

	if(token == ";")
	{
		ss << ';';
		if (metaData.transpillerOption.addEndLines)
			ss << '\n';
		return ss.str();
	}

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	ss << " = ";

	if (!iterator.nextToken())
		return ERROR_convertVarInit_outOfBounds(callFunc, iterator);

	Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo, scope, varSetter_Option::endSemiColon);
	if (varSetterResult.hasError)
		return varSetterResult.error;

	ss << varSetterResult.value();
	return ss.str();
}