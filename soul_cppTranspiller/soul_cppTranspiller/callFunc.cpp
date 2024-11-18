#include "callFunc.h"
#include <sstream>
#include <algorithm>

#include "scope.h"
#include "soulCheckers.h"

using namespace std;

#define ERROR_callArg ErrorInfo("function caller incomplete", iterator.currentLine)

static inline Result<ArgumentInfo> getArgInfo(uint32_t argPosition, const FuncInfo& callFunc)
{
	if(argPosition > callFunc.args.size())
		return ErrorInfo("not found", 0);

	return callFunc.args.at(argPosition);
}

static inline Result<vector<pair<string, uint32_t>>> getCallArgument(TokenIterator& iterator, MetaData& metaData, const FuncInfo& inCurrentFunc, const FuncInfo& callFunc, uint32_t& currentArgument, bool& lastArgument)
{
	stringstream ss;
	string token;

	vector<pair<string, uint32_t>> args;

	uint32_t openBracketStack = 0;
	lastArgument = false;

	while(iterator.nextToken(/*out*/token))
	{
		if(token == "(")
		{
			openBracketStack++;
			ss << '(';
			continue;
		}
		else if(token == ")")
		{
			openBracketStack--;
			ss << ')';
			if (openBracketStack == 0)
				return ERROR_callArg;
			continue;
		}

		Result<ArgumentInfo> argResult = getArgInfo(currentArgument, callFunc);
		if (argResult.hasError)
			return ErrorInfo("to many arguments", iterator.currentLine);

		ArgumentInfo argInfo = argResult.value();

		while (true)
		{
			if (token == "out")
			{
				if (!argType_isOut(argInfo.argType))
					return ErrorInfo("argument contains \'out\' without it being a out typed argument", iterator.currentLine);

				ss << token;

				if (!iterator.nextToken(token))
					return ERROR_callArg;
			}

			Result<VarInfo> varResult = getVarFromScope(inCurrentFunc, token);
			if (!varResult.hasError)
			{
				VarInfo varInfo = varResult.value();
				if (getDuckType(varInfo.type) != getDuckType(argInfo.valueType))
					return ErrorInfo("type given incorrect for argument: \'" + string(argInfo.name) + "\', typeGiven: \'" + toString(varInfo.type) + "\', argType: \'" + toString(argInfo.valueType) + '\'', iterator.currentLine);
			}
			else
			{
				if (!checkValue(token, argInfo.valueType))
					return ErrorInfo("type given incorrect for argument: \'" + string(argInfo.name) + "\', givenArgument: \'" + token + "\', argType: \'" + toString(argInfo.valueType) + '\'', iterator.currentLine);
			}

			ss << token;
			if (!iterator.nextToken(token))
				return ERROR_callArg;

			if (token == ",")
			{
				ss << ", ";
				args.emplace_back(ss.str(), currentArgument);
			}
			else if (token == ")")
			{
				ss << ")";
				args.emplace_back(ss.str(), currentArgument);
				return args;
			}

			if (!argInfo.canBeMultiple)
				break;
		}
	}

	return ERROR_callArg;
}


Result<string> convertFuncCall(TokenIterator& iterator, MetaData& metaData, FuncInfo callFunc, FuncInfo funcInfo)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token, /*step:*/0))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (token != callFunc.funcName)
		return ErrorInfo("function call, functionName invalid", iterator.currentLine);
	ss << token;

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (token != "(")
		return ErrorInfo("function call has to start with \'(\'", iterator.currentLine);

	ss << '(';

	uint32_t openBracketStack = 1;
	uint32_t argCounter = 1;
	vector<pair<string, uint32_t>> argsStrings;
	argsStrings.reserve(callFunc.args.size());

	uint32_t currentArgument = 0;
	bool lastArgument = false;
	while(lastArgument)
	{
		Result<vector<pair<string, uint32_t>>> argResult = getCallArgument(iterator, metaData, funcInfo, callFunc, /*out*/currentArgument, /*out*/lastArgument);
		if (argResult.hasError)
			return argResult.error;

		for (const auto& pair : argResult.value())
			argsStrings.push_back(pair);
		
		currentArgument++;
	}

	//sort
	//(
	//	argsStrings.begin(), argsStrings.end(),
	//	[](const pair<string, uint32_t>& a, const pair<string, uint32_t>& b)
	//	{
	//		return a.second < b.second;
	//	}
	//);

	for (const auto& pair : argsStrings)
		ss << pair.first;
	ss << ')';
	return ss.str();
}
