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

static inline ErrorInfo ERROR_convertFunctionCall_outOfBounds(FuncInfo& callFunc, TokenIterator& iterator)
{
    return ErrorInfo("incomplete functionBody funcName: \'" + callFunc.funcName + "\'", iterator.currentLine);
}

static inline Result<ArgumentInfo> _getArgInfo(uint32_t argPosition, const FuncInfo& callFunc)
{
	if (argPosition > callFunc.args.size())
		return ErrorInfo("not found", 0);

	return callFunc.args.at(argPosition);
}

static inline Result< vector<ArgumentPair> > _getCallArgument
(
	TokenIterator& iterator, 
	MetaData& metaData, 
	FuncInfo& inCurrentFunc, 
	FuncInfo& callFunc, 
	ScopeIterator& scope, 
	uint32_t& currentArgument, 
	bool& lastArgument
)
{
	stringstream ss;
	string& token = iterator.currentToken;

	vector<ArgumentPair> args;
	args.reserve(callFunc.args.size());

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
				return args;
			}
			continue;
		}

		Result<ArgumentInfo> argResult = _getArgInfo(currentArgument, callFunc);
		if (argResult.hasError)
			return ErrorInfo("to many arguments", iterator.currentLine);

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

			Result<string> varSetResult = convertVarSetter(iterator, metaData, argInfo.valueType, callFunc, scope, varSetter_Option::endComma_or_RoundBrasket, openBracketStack, /*addEndl:*/false);
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
					return args;
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
				break;

			if (!iterator.nextToken())
				break;
		}
	}

	return ERROR_convertFunctionCall_outOfBounds(callFunc, iterator);
}

Result<std::string> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, FuncInfo& callFunc, FuncInfo& funcInfo)
{
    stringstream ss;
    string& token = iterator.currentToken;

    if(token != callFunc.funcName)
        return ErrorInfo("function call, functionName invalid", iterator.currentLine);
    ss << token;

    if (!iterator.nextToken())
        return ERROR_convertFunctionCall_outOfBounds(callFunc, iterator);

    if (token != "(")
        return ErrorInfo("function call has to start with \'(\'", iterator.currentLine);

    ss << '(';

	uint32_t argCounter = 1;
	vector<ArgumentPair> argsStrings;
	argsStrings.reserve(callFunc.args.size());

	uint32_t currentArgument = 0;
	bool lastArgument = false;
	while (!lastArgument)
	{
		Result< vector<ArgumentPair> > argResult = _getCallArgument(iterator, metaData, funcInfo, callFunc, scope, /*out*/currentArgument, /*out*/lastArgument);
		if (argResult.hasError)
			return argResult.error;

		for (const auto& pair : argResult.value())
			argsStrings.push_back(pair);

		currentArgument++;
	}

	for (const auto& pair : argsStrings)
		ss << pair.argument;
	return ss.str();
}
