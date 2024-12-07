#include "convertFunctionCall.h"
#include "soulCheckers.h"

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

static inline Result<ArgumentInfo> getArgInfo(uint32_t argPosition, const FuncInfo& callFunc)
{
	if (argPosition > callFunc.args.size())
		return ErrorInfo("not found", 0);

	return callFunc.args.at(argPosition);
}

static inline Result< vector<ArgumentPair> > getCallArgument
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

	uint32_t openBracketStack = 1;
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
			if (openBracketStack == 0)
			{
				lastArgument = true;
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
				return args;
			}
			continue;
		}

		Result<ArgumentInfo> argResult = getArgInfo(currentArgument, callFunc);
		if (argResult.hasError)
			return ErrorInfo("to many arguments", iterator.currentLine);

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

			FuncInfo argCallFunc;
			Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
			if(metaData.TryGetfuncInfo(token, /*out*/argCallFunc))
			{
				TypeInfo& argumentType = argInfo.valueType;
				Result<void> isCompatible = argumentType.areTypesCompatiple(argCallFunc.returnType, iterator.currentLine);
				if (isCompatible.hasError)
					return isCompatible.error;

				Result<string> callResult = convertFunctionCall(iterator, metaData, scope, argCallFunc, inCurrentFunc);
				if (callResult.hasError)
					return callResult.error;

				ss << callResult.value();
			}
			else if(!varResult.hasError)
			{
				TypeInfo type = varResult.value().type;
				Result<void> isCompatible = type.areTypesCompatiple(argInfo.valueType, iterator.currentLine);
				if (isCompatible.hasError)
					return isCompatible.error;

				ss << token;
			}
			else
			{
				if(!checkValue(token, argInfo.valueType))
					return ErrorInfo("type given incorrect for argument: \'" + string(argInfo.name) + "\', givenArgument: \'" + token + "\', argType: \'" + toString(argInfo.valueType) + '\'', iterator.currentLine);

				ss << token;
			}

			if (!iterator.nextToken())
				break;

			if (token == ")")
			{
				ss << ")";
				openBracketStack--;
				if (openBracketStack <= 0)
				{
					lastArgument = true;
					args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
					return args;
				}

				if (!iterator.skipToken())
					break;
			}

			if (token == ",")
			{
				ss << ", ";
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
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

	uint32_t openBracketStack = 1;
	uint32_t argCounter = 1;
	vector<ArgumentPair> argsStrings;
	argsStrings.reserve(callFunc.args.size());

	uint32_t currentArgument = 0;
	bool lastArgument = false;
	while (!lastArgument)
	{
		Result< vector<ArgumentPair> > argResult = getCallArgument(iterator, metaData, funcInfo, callFunc, scope, /*out*/currentArgument, /*out*/lastArgument);
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
