#include "convertForLoop.h"
#include "varInit.h"
#include "funcBody.h"
#include "varSetter.h"
#include "convertVar.h"

#define ERROR_convertForLoop ErrorInfo("forLoop end enexpected", iterator.currentLine)
using namespace std;


static inline Result<void*> convertForLoop_firstCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_convertForLoop;

	if (token == ";")
	{
		ss << ";";
		return {};
	}

	if (token == "const")
		return ErrorInfo("iterator variable of forLoop can not be const", iterator.currentLine);

	Result<string> varResult;
	Type type = getType(token);
	Result<VarInfo> varInfo = scope.tryGetVariable_fromCurrent(token, metaData.globalScope);

	if (type != Type::invalid)
	{
		if (getDuckType(type) != DuckType::number)
			return ErrorInfo("iterator variable of forLoop has to by of a number type currentType: \'" + toString(type) + "\'", iterator.currentLine);

		FuncInfo _;
		varResult = convertVarInit(type, false, iterator, metaData, _, funcInfo, scope);
	}
	else if (!varInfo.hasError)
	{
		varResult = convertVar(varInfo.value(), iterator, metaData, funcInfo, scope);
	}
	else
	{
		return ErrorInfo("first forloop condition invalid token: \'" + token + "\'", iterator.currentLine);
	}

	if (varResult.hasError)
		return varResult.error;

	ss << varResult.value();
	return {};
}

static inline Result<void*> convertForLoop_secondCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_convertForLoop;

	if (token == ";")
		return ErrorInfo("second condition of forLoop can't be empty", iterator.currentLine);

	Result<string> varSetterResult = convertVarSetter(iterator, metaData, Type::bool_, funcInfo, scope, varSetter_Option::endSemiColon);
	if (varSetterResult.hasError)
		return varSetterResult.error;

	ss << varSetterResult.value();
	return {};
}

static inline Result<void*> convertForLoop_thirdCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_convertForLoop;

	if (token == ";")
		return {};

	if (token == "++" || token == "--")
	{
		if (!iterator.peekToken(/*out*/token))
			return ERROR_convertForLoop;
	}

	Result<VarInfo> varInfoResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope);
	if (varInfoResult.hasError)
		return varInfoResult.error;

	Result<string> varResult = convertVar(varInfoResult.value(), iterator, metaData, funcInfo, scope, varSetter_Option::endRoundBracket);
	if (varResult.hasError)
		return varResult.error;

	ss << varResult.value();
	return {};
}

Result<string> convertForLoop(TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token, /*steps:*/0))
		return ERROR_convertForLoop;

	if (token != "(")
		return ErrorInfo("forLoop needs to start with \'(\', starts with: \'" + token + "\'", iterator.currentLine);
	ss << '(';

	Result<void*> result;
	result = convertForLoop_firstCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
	if (result.hasError)
		return result.error;

	result = convertForLoop_secondCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
	if (result.hasError)
		return result.error;

	result = convertForLoop_thirdCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
	if (result.hasError)
		return result.error;

	return ss.str();
}
