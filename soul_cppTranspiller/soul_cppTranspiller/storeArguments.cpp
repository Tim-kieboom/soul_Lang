#include "storeArguments.h"
#include "soulChecker.h"

using namespace std;

struct StoreArgsInfo
{
    ArgumentType argType = ArgumentType::default_;
    uint32_t openBracketCounter = 1;
    string defaultValue = "";
    bool isOptional = false;
    string argName = "";
    RawType type;

    uint64_t argumentPosition = 1;

    void reset()
    {
        this->type;
        this->argName = "";
        this->defaultValue = "";
        this->argumentPosition++;
        this->isOptional = false;
        this->argType = ArgumentType::default_;
    }
};

static inline Result<void> storeArgument
(
    TokenIterator& iterator,
    StoreArgsInfo& storeInfo,
    MetaData& metaData,
    vector<ArgumentInfo>& args
)
{
    RawType& type = storeInfo.type;
    string& argName = storeInfo.argName;
    ArgumentType& argType = storeInfo.argType;
    uint64_t& argumentPosition = storeInfo.argumentPosition;

    if(!type.isValid(metaData.classStore))
        return ErrorInfo("valueType of argument is invalid argumentNumber: \'" + argumentPosition + string("\'"), iterator.currentLine);

    if (argName.empty())
        return ErrorInfo("no name given to argument, argument type: \'" + toString(type) + "\'", iterator.currentLine);

    ArgumentInfo arg = ArgumentInfo(storeInfo.isOptional, type, argName, argType);
    args.push_back(arg);

    storeInfo.reset();
    return {};
}

static inline Result<RawType> storeLastArgument
(
    TokenIterator& iterator,
    StoreArgsInfo& storeInfo,
    MetaData& metaData,
    vector<ArgumentInfo>& args
)
{
    string& token = iterator.currentToken;
    if (storeInfo.type.isValid(metaData.classStore))
    {
        Result<void> result = storeArgument(iterator, storeInfo, metaData, args);
        if (result.hasError)
            return result.error;
    }

    RawType& type = storeInfo.type;
    if (!iterator.nextToken())
        return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

    if (token != ":")
        return ErrorInfo("function Declarations doesn't end with ':'", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

    Result<RawType> returnType = getRawType(iterator, metaData.classStore);
    if (returnType.hasError)
        return returnType.error;

    return returnType.value();
}


Result<vector<ArgumentInfo>> storeArguments(TokenIterator& iterator, MetaData& metaData, RawType& returnType)
{
    StoreArgsInfo storeInfo;
    uint32_t& openBracketCounter = storeInfo.openBracketCounter;
    vector<ArgumentInfo> args;

    string& token = iterator.currentToken;
    while(iterator.nextToken())
    {
        Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
        if (token == ",")
        {
            Result<void> result = storeArgument(iterator, storeInfo, metaData, args);
            if (result.hasError)
                return result.error;
        }
        else if(token == "(")
        {
            openBracketCounter++;
        }
        else if(token == ")")
        {
            if(openBracketCounter <= 1)
            {
                Result<RawType> result = storeLastArgument(iterator, storeInfo, metaData, args);
                if (result.hasError)
                    return result.error;

                returnType = result.value();
                return args;
            }

            openBracketCounter--;
        }
        else if(getArgumentType(token) != ArgumentType::invalid)
        {
            storeInfo.argType = getArgumentType(token);
        }
        else if(token == "=")
        {
            storeInfo.isOptional = true;
        }
        else if (!typeResult.hasError)
        {
            storeInfo.type = typeResult.value();
        }
        else
        {
            if (!checkName(token))
                return ErrorInfo("argument name is not allowed name: \'" + token + "\'", iterator.currentLine);
            storeInfo.argName = token;
        }
    }
    return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);
}
