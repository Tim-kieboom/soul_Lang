#include "convertBody.h"
#include "convertVar.h"
#include "convertVarInit.h"
#include "convertForLoop.h"
#include "convertVarSetter.h"
#include "convertFunctionCall.h"

using namespace std;
initializer_list<const char*> bodiedStatements = { "if", "else", "while", "for" };

static inline ErrorInfo ERROR_convertBody_outOfBounds(FuncInfo& funcInfo, TokenIterator& iterator)
{
	return ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);
}

static inline Result<string> convertBodiedStetement(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData, ScopeIterator& scope, uint32_t depth)
{
	stringstream ss;
	string& token = iterator.currentToken;
	string bodiedStatment = token;

	if (!iterator.nextToken())
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (bodiedStatment == "else" && token == "if")
	{
		bodiedStatment = "else if";
		if (!iterator.nextToken())
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);
	}

	uint64_t statementNestingIndex = funcInfo.scope.size();
	funcInfo.scope.emplace_back
	(
		Nesting::makeChild(&scope.getCurrentNesting(), funcInfo.scope)
	);
	ScopeIterator statementScope = ScopeIterator(funcInfo.scope, statementNestingIndex);

	bool isConditionalStatement = !initListEquals({ "else" }, bodiedStatment);
	if(isConditionalStatement)
	{
		if(token != "(")
			return ErrorInfo("bodiedStatement: \'" + token + "\' doesn't start with '('", iterator.currentLine);
		
		Result<string> conditionResult;
		if (bodiedStatment == "for")
		{
			conditionResult = convertForLoop(iterator, metaData, funcInfo, statementScope);
		}
		else
		{
			static const TypeInfo boolType = TypeInfo(PrimitiveType::bool_);
			conditionResult = convertVarSetter(iterator, metaData, boolType, funcInfo, statementScope, varSetter_Option::endRoundBracket);
		}

		if (conditionResult.hasError)
			return conditionResult.error;
		ss << bodiedStatment << conditionResult.value();
	}
	else
	{
		if (!iterator.nextToken(/*steps:*/-1))
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);

		ss << bodiedStatment;
	}

	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> bodyResult = convertBody(iterator, funcInfo, metaData, statementScope, depth+1);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}	

Result<string> convertBody
(
	TokenIterator& iterator, 
	FuncInfo& funcInfo, 
	MetaData& metaData, 
	ScopeIterator& scope,
	uint32_t depth
)
{
	stringstream ss;
	string& token = iterator.currentToken;
	
	string checkBracket;
	if (!iterator.peekToken(checkBracket))
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (checkBracket != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);

	if (metaData.transpillerOption.addEndLines)
	{
		for (uint32_t i = 0; i < depth - 1; i++)
			ss << '\t';
	}

	ss << '{';
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> result;
	uint32_t openCurlyBracketCounter = 0;
	while(iterator.nextToken())
	{

		if (iterator.currentLine == 152)
			int debug = 0;

		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}
		else if (token == "}")
		{
			openCurlyBracketCounter--;

			if (openCurlyBracketCounter != 0)
				continue;

			if (metaData.transpillerOption.addEndLines)
			{
				for (uint32_t i = 0; i < depth - 1; i++)
					ss << '\t';
			}
			ss << '}';
			if (metaData.transpillerOption.addEndLines)
				ss << '\n';

			return ss.str();
		}

		if (metaData.transpillerOption.addEndLines)
		{
			for (uint32_t i = 0; i < depth; i++) 
				ss << '\t';
		}

		FuncInfo callFunc;
		Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
		Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

		if(metaData.TryGetfuncInfo(token, /*out*/callFunc))
		{
			result = convertFunctionCall(iterator, metaData, scope, callFunc, funcInfo);
			if (result.hasError)
				return result.error;

			ss << result.value();

			if (!iterator.nextToken())
				return ERROR_convertBody_outOfBounds(funcInfo, iterator);

			if(token != ";")
				return ErrorInfo("invalid symbol in argument, symbool: \'" + token + '\'', iterator.currentLine);

			ss << ';';
			if (metaData.transpillerOption.addEndLines)
				ss << '\n';
		}
		else if(!typeResult.hasError)
		{
			TypeInfo type = typeResult.value();
			result = convertVarInit(type, iterator, metaData, callFunc, funcInfo, scope);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else if (token == "return")
		{
			if (!iterator.nextToken())
				break;

			result = convertVarSetter(iterator, metaData, funcInfo.returnType, funcInfo, scope, varSetter_Option::endSemiColon);
			if (result.hasError)
				return result.error;

			ss << "return " << result.value();
		}
		else if(initListEquals(bodiedStatements, token))
		{
			result = convertBodiedStetement(iterator, funcInfo, metaData, scope, depth);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else if (!varResult.hasError)
		{
			result = convertVar(varResult.value(), iterator, metaData, funcInfo, scope);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else
		{
			return ErrorInfo("unknown token: \'" + token + '\'', iterator.currentLine);
		}
	}

	return ERROR_convertBody_outOfBounds(funcInfo, iterator);
}
