#include "funcBody.h"
#include <sstream>
#include <algorithm>

#include "scope.h"
#include "soulCheckers.h"
#include "cppConverter.h"

using namespace std;

static inline Result<string> convertVar(Result<VarInfo>& varResult, TokenIterator& iterator, FuncInfo& funcInfo)
{
	VarInfo varInfo = varResult.value();
	stringstream ss;

	string token;
	string symbool;

	if (!iterator.nextToken(/*out*/symbool))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (!initListEquals({ "+=", "-=", "*=", "/=", "=", "++", "--" }, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

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

static inline Result<string> convertVarInit(Type type, TokenIterator& iterator, FuncInfo& funcInfo)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (!checkName(token))
		return ErrorInfo("name invalid name: " + token, iterator.currentLine);

	ss << typeToCppType(type) << ' ' << token;

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (token == ";")
	{
		ss << ';';
		return ss.str();
	}

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: " + token, iterator.currentLine);

	ss << " = ";

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	ss << "";//convertVarSetter(tokens, var.type, /*out*/ i, /*out*/ currentLine, funcInfo, constStringStore, funcStore);
	return ss.str();
}

Result<string> convertFunctionBody(TokenIterator& iterator, FuncInfo& funcInfo, MetaData& metaData)
{
	stringstream ss("{\n");

	string token;
	if (!iterator.peekToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);

	if (token != "{")
		return ErrorInfo("no '{' after function declaration, funcName: " + string(funcInfo.funcName), iterator.currentLine);

	uint32_t openCurlyBracketCounter = 0;

	while(iterator.nextToken(/*out*/token))
	{
		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}

		if(token == "}")
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
			ss << "";//convertFuncitonCall

			if (!iterator.nextToken(/*out*/token))
				return ErrorInfo("incomplete functionBody funcName: " + string(callFunc.funcName), iterator.currentLine);;

			if (token != ";")
				return ErrorInfo("invalid symbol in argument, symbool: " + token, iterator.currentLine);

			ss << ";\n";
		}
		else if (!varResult.hasError)
		{
			Result<string> result = convertVar(varResult, iterator, funcInfo);
			if (result.hasError)
				return result;

			ss << result.value();
		}
		else if(type != Type::invalid)
		{
			Result<string> result = convertVarInit(type, iterator, callFunc);
			if (result.hasError)
				return result;

			ss << result.value();
		}
		else if(token == "return")
		{
			if (!iterator.nextToken(/*out*/token))
				break;
			ss << "return"; //convertVarSetter(tokens, var.type, /*out*/ i, /*out*/ currentLine, funcInfo, constStringStore, funcStore);
		}
	}

	return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);
}
