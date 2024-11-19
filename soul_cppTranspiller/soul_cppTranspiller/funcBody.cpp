#include "funcBody.h"
#include <sstream>
#include <algorithm>

#include "scope.h"
#include "varSetter.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "callFunc.h"

using namespace std;

initializer_list<const char*> bodiedStatements = { "if", "else", "while", "for" };

#define ERROR_var ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine)
#define ERROR_varInit ErrorInfo("incomplete functionBody funcName: \'" + string(callFunc.funcName) + '\'', iterator.currentLine)
#define ERROR_funcBody ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);

static inline Result<string> convertVar(VarInfo& varInfo, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo)
{
	stringstream ss;

	string token;
	string symbool;

	if (!iterator.nextToken(/*out*/symbool))
		return ERROR_var;

	if (!initListEquals({ "+=", "-=", "*=", "/=", "=", "++", "--" }, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (!iterator.nextToken(/*out*/token))
		return ERROR_var;

	if (initListEquals({ "++", "--" }, symbool))
	{
		if (token != ";")
			return ErrorInfo("variable incomplete atfer \'" + symbool + "\' need ';' but has: " + token, iterator.currentLine);

		ss << varInfo.name << symbool << ";\n";
		return ss.str();
	}

	Result<string> varSetResult = convertVarSetter(iterator, metaData, varInfo.type, funcInfo, varSetter_Option::endSemiColon);
	if (varSetResult.hasError)
		return varSetResult.error;

	ss << varInfo.name << ' ' << symbool << ' ' << varSetResult.value();
	return ss.str();
}

static inline Result<string> convertVarInit(Type type, bool isMutable, TokenIterator& iterator, MetaData& metaData, FuncInfo& callFunc, FuncInfo& funcInfo)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_varInit;

	if (!checkName(token))
		return ErrorInfo("name invalid name: " + token, iterator.currentLine);

	ss << typeToCppType(type) << ' ' << token;

	funcInfo.scope.emplace_back(string_copyTo_c_str(token), type, isMutable);

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

	Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, callFunc, varSetter_Option::endSemiColon);
	if (varSetterResult.hasError)
		return varSetterResult.error;

	ss << varSetterResult.value();
	return ss.str();
}

static inline Result<string> convertBodiedStatement(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData)
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

	bool isConditionalStatement = !initListEquals({ "else" }, bodiedStatment);
	if (isConditionalStatement)
	{
		if (token != "(")
			return ErrorInfo("bodiedStatement: \'" + token + "\' doesn't start with '('", iterator.currentLine);

		Result<string> varSetterResult = convertVarSetter(iterator, metaData, Type::bool_, funcInfo, varSetter_Option::endRoundBracket);
		if (varSetterResult.hasError)
			return varSetterResult.error;

		ss << bodiedStatment << varSetterResult.value();
	}
	else
	{
		if (!iterator.nextToken(/*out*/token, /*step:*/-1))
			return ERROR_funcBody;
	
		ss << bodiedStatment;
	}

	Result<string> bodyResult = convertFunctionBody(iterator, funcInfo, metaData);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}

Result<string> convertFunctionBody(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData)
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
		if (iterator.currentLine >= 111)
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
		Result<VarInfo> varResult = getVarFromScope(funcInfo, metaData, token);

		if (metaData.TryGetfuncInfo(token, /*out*/callFunc))
		{
			Result<string> funcCallResult = convertFuncCall(iterator, metaData, funcInfo.scope, callFunc, funcInfo);
			if (funcCallResult.hasError)
				return funcCallResult.error;

			ss << funcCallResult.value();

			if (!iterator.nextToken(/*out*/token))
				return ERROR_funcBody;

			if (token != ";")
				return ErrorInfo("invalid symbol in argument, symbool: \'" + token + '\'', iterator.currentLine);

			ss << ";\n";
		}
		else if (!varResult.hasError)
		{
			Result<string> result = convertVar(varResult.value(), iterator, metaData, funcInfo);
			if (result.hasError)
				return result.error;

			ss << result.value();
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

			Result<string> result = convertVarInit(type, isMutable, iterator, metaData, callFunc, /*out*/funcInfo);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else if(token == "return")
		{
			if (!iterator.skipToken())
				break;

			Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo, varSetter_Option::endSemiColon);
			if (varSetterResult.hasError)
				return varSetterResult.error;

			ss << "return " << varSetterResult.value();
		}
		else if (initListEquals(bodiedStatements, token))
		{
			Result<string> statementResult = convertBodiedStatement(iterator, funcInfo, metaData);
			if (statementResult.hasError)
				return statementResult.error;

			ss << statementResult.value();
		}
		else
		{
			return ErrorInfo("unknown token: \'" + token + '\'', iterator.currentLine);
		}
	}

	return ERROR_funcBody;
}
