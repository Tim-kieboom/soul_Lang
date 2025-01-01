#include "convertFunctionCall.h"
#include "soulCheckers.h"
#include "convertVarSetter.h"

using namespace std;

struct ArgumentPair
{
	string argument;
	uint64_t argumentPosition = 0;

	ArgumentPair() = default;
	ArgumentPair(string&& str, uint64_t pos)
		: argument(str), argumentPosition(pos)
	{
	}
};

struct CallArgInfo
{
	vector<ArgumentPair> args;
	uint64_t skipedTokens = 0;
	
	CallArgInfo() = default;
	CallArgInfo(vector<ArgumentPair>& args, uint64_t skipedTokens)
		: args(args), skipedTokens(skipedTokens)
	{
	}
};

static inline ErrorInfo ERROR_convertFunctionCall_outOfBounds(string& callFuncName, TokenIterator& iterator)
{
    return ErrorInfo("incomplete functionBody funcName: \'" + callFuncName + "\'", iterator.currentLine);
}

static inline Result<ArgumentInfo> _getArgInfo(uint32_t argPosition, const FuncInfo& callFunc)
{
	if(argPosition == 0)
		return ErrorInfo("argPosition can not be null", 0);


	if (argPosition > callFunc.args.size())
		return ErrorInfo("not found", 0);

	return callFunc.args.at(argPosition-1);
}

static inline Result<CallArgInfo> _getCallArgument
(
	TokenIterator iterator, 
	MetaData& metaData, 
	FuncInfo& inCurrentFunc, 
	FuncInfo& callFunc,
	ScopeIterator& scope, 
	uint32_t& currentArgument, 
	string* className,
	bool& lastArgument
)
{
	uint64_t beginI = iterator.i;
	stringstream ss;
	string& token = iterator.currentToken;

	vector<ArgumentPair> args;
	args.reserve(6);

	int64_t openBracketStack = 1;
	lastArgument = false;

	while (iterator.nextToken())
	{
		if (token == "(")
		{
			openBracketStack++;
			ss << '(';
			continue;
		}
		else if (token == ")")
		{
			openBracketStack--;
			ss << ')';
			if (openBracketStack <= 0)
			{
				lastArgument = true;
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
				return CallArgInfo(args, iterator.i - beginI);
			}
			continue;
		}
		else
		{
			break;
		}
	}

	Result<ArgumentInfo> argResult = _getArgInfo(currentArgument, callFunc);
	if (argResult.hasError)
		return ErrorInfo(argResult.error.message + "\ncould not get argument", iterator.currentLine);

	string prevToken;
	ArgumentInfo argInfo = argResult.value();
	while(true)
	{
		if(argType_isOut(argInfo.argType))
		{
			if (token != "out")
				return ErrorInfo("argument: \'" + argInfo.name + "\' needs \'out\' when calling function", iterator.currentLine);

			ss << "/*out*/";
			if (!iterator.nextToken())
				break;
		}

		Result<string> varSetResult = convertVarSetter(iterator, metaData, argInfo.valueType, callFunc, scope, varSetter_Option::endComma_or_RoundBrasket, openBracketStack, className, /*addEndl:*/false);
		if (varSetResult.hasError)
			return varSetResult.error;
		ss << varSetResult.value();

		prevToken = token;
		if (!iterator.nextToken())
			break;

		if(prevToken == ")")
		{
			openBracketStack--;
			if (openBracketStack <= 0)
			{
				if (!iterator.nextToken(/*step:*/-1))
					break;

				lastArgument = true;
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
				return CallArgInfo(args, iterator.i - beginI);
			}
			continue;
		}
		else if (token == ",")
		{
			args.emplace_back(ArgumentPair(ss.str(), currentArgument));
			ss.str("");
		}
		else
		{
			return ErrorInfo("argument in funcCall has to end with ',' or ')'", iterator.currentLine);
		}

		if (!argInfo.canBeMultiple)
			return CallArgInfo(args, iterator.i - beginI);

		if (!iterator.nextToken())
			break;
		
	}

	return ERROR_convertFunctionCall_outOfBounds(callFunc.funcName, iterator);
}

static inline Result< pair<vector<ArgumentPair>, FuncInfo> > _getAllArguments
(
	TokenIterator& iterator,
	MetaData& metaData,
	FuncInfo& inCurrentFunc,
	vector<FuncInfo>& callFuncs,
	string& funcCallName,
	ScopeIterator& scope,
	uint32_t& currentArgument,
	string* className,
	bool& lastArgument
)
{
	if (callFuncs.empty())
		return ErrorInfo("no function found to match with functionCall", iterator.currentLine);

	uint64_t i = 0;
	Result<CallArgInfo> argResult;
	for (FuncInfo& callFunc : callFuncs)
	{
		argResult = _getCallArgument(iterator, metaData, inCurrentFunc, callFunc, scope, /*out*/currentArgument, className, /*out*/lastArgument);

		if (!argResult.hasError)
		{
			CallArgInfo& argInfo = argResult.value();
			if (!iterator.nextToken(/*steps:*/argInfo.skipedTokens))
				return ERROR_convertFunctionCall_outOfBounds(funcCallName, iterator);

			return make_pair(argInfo.args, callFunc);
		}
	}

	return ErrorInfo(argResult.error.message + "\nfunction decl of: \'" + callFuncs.at(0).funcName + "\', does not match args in call", iterator.currentLine);
}

Result<std::string> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, string& _callFuncName, FuncInfo& funcInfo, string* className)
{
    stringstream ss;
    string& token = iterator.currentToken;
	string callFuncName = _callFuncName;

    ss << token;

	vector<FuncInfo> callFuncs;
	if (metaData.isFunction(callFuncName))
	{
		callFuncs = metaData.funcStore[callFuncName];
	}
	else if(className != nullptr && metaData.isMethode(callFuncName, *className))
	{

	}
	else
	{
		return ErrorInfo("function: \'" + callFuncName + "\', is not found", iterator.currentLine);
	}

    if (!iterator.nextToken())
        return ERROR_convertFunctionCall_outOfBounds(callFuncName, iterator);

    if (token != "(")
        return ErrorInfo("function call has to start with \'(\'", iterator.currentLine);

    ss << '(';

	uint32_t argCounter = 1;
	vector<ArgumentPair> argsStrings;
	argsStrings.reserve(6);

	FuncInfo callFunc;

	uint32_t currentArgument = 1;
	bool lastArgument = false;
	while (!lastArgument)
	{
		Result< pair<vector<ArgumentPair>, FuncInfo> > argResult = _getAllArguments(iterator, metaData, funcInfo, callFuncs, callFuncName, scope, currentArgument, className, lastArgument);
		if (argResult.hasError)
			return argResult.error;

		callFunc = argResult.value().second;
		for (const auto& pair : argResult.value().first)
			argsStrings.push_back(pair);

		currentArgument++;
	}

	if (argsStrings.size() < callFunc.args.size())
		return ErrorInfo("function call has to little arguments, funcName: \'" +callFuncName+ "\'", iterator.currentLine);


	for (const auto& pair : argsStrings)
		ss << pair.argument;
	return ss.str();
}
