#include "funcBody.h"
#include <sstream>
#include <algorithm>

#include "scope.h"
#include "VarInit.h"
#include "callFunc.h"
#include "varSetter.h"
#include "varSetter.h"
#include "convertVar.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "convertForLoop.h"

using namespace std;

initializer_list<const char*> bodiedStatements = { "if", "else", "while", "for" };

#define ERROR_funcBody ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine)

static inline Result<string> convertBodiedStatement(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData, ScopeIterator& scope)
{
	stringstream ss;

	string token;
	if (!iterator.peekToken(/*out*/token, /*step:*/0))
		return ERROR_funcBody;

	string bodiedStatment = token;

	string nextToken;
	if (!iterator.nextToken(/*out*/nextToken))
		return ERROR_funcBody;

	if (token == "else" && nextToken == "if")
	{
		bodiedStatment = "else if";
		if (!iterator.nextToken(/*out*/token))
			return ERROR_funcBody;
	}
	else
	{
		token = nextToken;
	}

	uint64_t statementNestingIndex = funcInfo.scope.size();
	funcInfo.scope.emplace_back
	(
		Nesting::makeChild(&scope.getCurrentNesting(), funcInfo.scope)
	);
	ScopeIterator statementScope = ScopeIterator(funcInfo.scope, statementNestingIndex);

	bool isConditionalStatement = !initListEquals({ "else" }, bodiedStatment);
	if (isConditionalStatement)
	{
		if (token != "(")
			return ErrorInfo("bodiedStatement: \'" + token + "\' doesn't start with '('", iterator.currentLine);


		Result<string> conditionResult;
		if (bodiedStatment == "for")
		{
			conditionResult = convertForLoop(iterator, metaData, funcInfo, statementScope);
		}
		else
		{
			conditionResult = convertVarSetter(iterator, metaData, Type::bool_, funcInfo, statementScope, varSetter_Option::endRoundBracket);
		}

		if (conditionResult.hasError)
			return conditionResult.error;
		ss << bodiedStatment << conditionResult.value();
	}
	else
	{
		if (!iterator.nextToken(/*out*/token, /*step:*/-1))
			return ERROR_funcBody;
	
		ss << bodiedStatment;
	}

	Result<string> bodyResult = convertFunctionBody(iterator, funcInfo, metaData, statementScope);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}

Result<string> convertFunctionBody(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData, ScopeIterator& scope)
{
	stringstream ss;

	string token;
	if (!iterator.peekToken(/*out*/token))
		return ERROR_funcBody;

	if (token != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);

	ss << "{\n";

	uint32_t openCurlyBracketCounter = 0;

	while(iterator.nextToken(/*out*/token))
	{
		if (iterator.currentLine == 169)
			int debug = 0;

		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}
		else if(token == "}")
		{
			openCurlyBracketCounter--;

			if (openCurlyBracketCounter != 0)
				continue;

			ss << "}\n";
			return ss.str();
		}

		ss << '\t';

		FuncInfo callFunc;
		Type type = getType(token);

		if (metaData.TryGetfuncInfo(token, /*out*/callFunc))
		{
			Result<string> funcCallResult = convertFuncCall(iterator, metaData, scope, callFunc, funcInfo);
			if (funcCallResult.hasError)
				return funcCallResult.error;

			ss << funcCallResult.value();

			if (!iterator.nextToken(/*out*/token))
				return ERROR_funcBody;

			if (token != ";")
				return ErrorInfo("invalid symbol in argument, symbool: \'" + token + '\'', iterator.currentLine);

			ss << ";\n";
			continue;
		}
		else if(type != Type::invalid || token == "const")
		{
			bool isMutable = true;
			if(token == "const")
			{
				isMutable = false;

				if (!iterator.nextToken(/*out*/token))
					return ERROR_funcBody;

				type = getType(token);
			}

			Result<string> result = convertVarInit(type, isMutable, iterator, metaData, callFunc, /*out*/funcInfo, scope);
			if (result.hasError)
				return result.error;

			ss << result.value();
			continue;
		}
		else if(token == "return")
		{
			if (!iterator.skipToken())
				break;

			Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo, scope, varSetter_Option::endSemiColon);
			if (varSetterResult.hasError)
				return varSetterResult.error;

			ss << "return " << varSetterResult.value();
			continue;
		}
		else if (initListEquals(bodiedStatements, token))
		{
			Result<string> statementResult = convertBodiedStatement(iterator, funcInfo, metaData, scope);
			if (statementResult.hasError)
				return statementResult.error;

			ss << statementResult.value();
			continue;
		}

		Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope);
		if (!varResult.hasError)
		{
			Result<string> result = convertVar(varResult.value(), iterator, metaData, funcInfo, scope);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else
		{
			return ErrorInfo("unknown token: \'" + token + '\'', iterator.currentLine);
		}
	}

	return ERROR_funcBody;
}
