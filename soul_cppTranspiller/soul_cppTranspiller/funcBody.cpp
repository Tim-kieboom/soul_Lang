#include "funcBody.h"
#include <sstream>
#include <algorithm>

#include "scope.h"
#include "varSetter.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "callFunc.h"

using namespace std;
#define ERROR_var ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine)
#define ERROR_varInit ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine)
#define ERROR_funcBody ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);

static inline Result<string> convertVar(Result<VarInfo>& varResult, TokenIterator& iterator, FuncInfo& funcInfo)
{
	VarInfo varInfo = varResult.value();
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

	ss << varInfo.name << ' ' << symbool << ' ' << "";//convertVarSetter(tokens, var.type, /*out*/ i, /*out*/ currentLine, funcInfo, constStringStore, funcStore);
	return ss.str();
}

static inline Result<string> convertVarInit(Type type, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ERROR_varInit;

	if (!checkName(token))
		return ErrorInfo("name invalid name: " + token, iterator.currentLine);

	ss << typeToCppType(type) << ' ' << token;

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

	Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo);
	if (varSetterResult.hasError)
		return varSetterResult.error;

	ss << varSetterResult.value();
	return ss.str();
}


Result<string> convertFunctionBody(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData)
{
	stringstream ss;
	ss << "{\n";

	string token;
	if (!iterator.peekToken(/*out*/token))
		return ERROR_funcBody;

	if (token != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);

	uint32_t openCurlyBracketCounter = 0;

	while(iterator.nextToken(/*out*/token))
	{
		if (iterator.currentLine == 92)
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
		Result<VarInfo> varResult = getVarFromScope(callFunc, token);

		if (metaData.TryGetfuncInfo(token, /*out*/callFunc))
		{
			Result<string> funcCallResult = convertFuncCall(iterator, metaData, callFunc, funcInfo);
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
			Result<string> result = convertVar(varResult, iterator, funcInfo);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else if(type != Type::invalid)
		{
			Result<string> result = convertVarInit(type, iterator, metaData, callFunc);
			if (result.hasError)
				return result.error;

			ss << result.value();
		}
		else if(token == "return")
		{
			if (!iterator.skipToken())
				break;

			Result<string> varSetterResult = convertVarSetter(iterator, metaData, type, funcInfo);
			if (varSetterResult.hasError)
				return varSetterResult.error;

			ss << "return " << varSetterResult.value();
		}
	}

	return ERROR_funcBody;
}
