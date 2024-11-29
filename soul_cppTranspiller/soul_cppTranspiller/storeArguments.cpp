#include "storeArguments.h"
#include "Type.h"

using namespace std;

struct StoreArgsInfo
{
	ArgumentType argType = ArgumentType::tk_default;
	Type type = Type::invalid;
	uint32_t openBracketCounter = 1;
	string defaultValue = "";
	string argName = "";

	uint64_t argumentPosition = 0;

	string token;
};

static inline Result<void*> storeArgument(const uint64_t currentline, StoreArgsInfo& storeArgsInfo, /*out*/ FuncInfo& funcInfo, /*out*/ Nesting& scope)
{
	string& token = storeArgsInfo.token;
	if (storeArgsInfo.type == Type::invalid)
		return ErrorInfo("valueType of argument is invalid token: " + token, currentline);

	Type& type = storeArgsInfo.type;
	string& _argName = storeArgsInfo.argName;
	ArgumentType& argType = storeArgsInfo.argType;
	uint64_t& argumentPosition = storeArgsInfo.argumentPosition;

	const char* argName = string_copyTo_c_str(_argName);
	auto argInfo = ArgumentInfo(argType, type, argName, argumentPosition);

	auto varInfo = VarInfo(argName, type, argType_isMutable(argType), argType_isOptions(argType));

	funcInfo.args.emplace_back(argInfo);
	scope.addVariable(varInfo);

	storeArgsInfo.argName = "";
	storeArgsInfo.defaultValue = "";
	storeArgsInfo.argumentPosition++;
	storeArgsInfo.type = Type::invalid;
	storeArgsInfo.argType = ArgumentType::tk_default;

	return {};
}

static inline Result<void*> storeLastArgument(StoreArgsInfo& storeArgsInfo, /*out*/ TokenIterator& iterator, /*out*/ MetaData& metaData, /*out*/ FuncInfo& funcInfo, /*out*/ Nesting& scope)
{
	string& token = storeArgsInfo.token;
	if (storeArgsInfo.type != Type::invalid)
	{
		Result<void*> result = storeArgument(iterator.currentLine, /*out*/storeArgsInfo, /*out*/funcInfo, /*out*/scope);
		if (result.hasError)
			return result;
	}

	Type& type = storeArgsInfo.type;

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

	if (token != ":")
		return ErrorInfo("function Declarations doesn't end with ':'", iterator.currentLine);

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

	Type returnType = getType(token);
	if (returnType == Type::invalid)
		return ErrorInfo("function Declarations unknown return type", iterator.currentLine);

	funcInfo.returnType = returnType;

	return {};
}

Result<void*> storeArguments(/*out*/ TokenIterator& iterator, /*out*/ MetaData& metaData, /*out*/ FuncInfo& funcInfo, /*out*/ Nesting& scope)
{
	StoreArgsInfo storeArgsInfo;
	string& token = storeArgsInfo.token;
	uint32_t& openBracketCounter = storeArgsInfo.openBracketCounter;

	while(iterator.nextToken(/*out*/token))
	{
		if (token == ",")
		{
			Result<void*> result = storeArgument(iterator.currentLine, /*out*/storeArgsInfo, /*out*/funcInfo, /*out*/scope);
			if (result.hasError)
				return result;
		}
		else if (token == "(")
		{
			openBracketCounter++;
		}
		else if (token == ")")
		{
			if (openBracketCounter <= 1)
			{
				Result<void*> result = storeLastArgument(storeArgsInfo, iterator, metaData, funcInfo, scope);
				if (result.hasError)
					return result;

				return {};
			}

			openBracketCounter--;
		}
		else if (getArgType_symbol(token) != ArgumentType::invalid)
		{
			storeArgsInfo.argType = getArgType_symbol(token);
		}
		else if (token == "=")
		{
			storeArgsInfo.argType = argType_ChangeToOptional(storeArgsInfo.argType);

			if (!iterator.nextToken(/*out*/token))
				break;
		}
		else if (getType(token) != Type::invalid)
		{
			storeArgsInfo.type = getType(token);
		}
		else
		{
			storeArgsInfo.argName = token;
		}
	}

	return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);
}

