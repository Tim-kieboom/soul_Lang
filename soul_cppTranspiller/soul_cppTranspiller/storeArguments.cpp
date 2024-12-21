#include "storeArguments.h"
#include "TypeInfo.h"
#include "soulCheckers.h"

using namespace std;

struct StoreArgsInfo
{
    ArgumentType argType = ArgumentType::tk_default;
    TypeInfo type;
    uint32_t openBracketCounter = 1;
    string defaultValue = "";
    string argName = "";

    uint64_t argumentPosition = 1;

    void reset()
    {
        this->argName = "";
        this->defaultValue = "";
        this->argumentPosition++;
        this->type = TypeInfo();
        this->argType = ArgumentType::tk_default;
    }
};

static inline Result<void> storeArgument
(
    TokenIterator& iterator,  
    StoreArgsInfo& storeInfo, 
    /*out*/ FuncInfo& funcInfo,
    /*out*/ Nesting& scope
)
{
    TypeInfo type = storeInfo.type;
    string& argName = storeInfo.argName;
    ArgumentType& argType = storeInfo.argType;
    uint64_t& argumentPosition = storeInfo.argumentPosition;

    if (!type.isValid())
        return ErrorInfo("valueType of argument is invalid argumentNumber: \'" + argumentPosition + string("\'"), iterator.currentLine);

    if (argName.empty())
        return ErrorInfo("no name given to argument, argument type: \'" + toString(type) + "\'", iterator.currentLine);

    ArgumentInfo argInfo = ArgumentInfo(argType, type, argName, argumentPosition);
    VarInfo varInfo = VarInfo(argName, type, argType_isOptions(argType));

    funcInfo.args.emplace_back(argInfo);
    scope.addVariable(varInfo);

    storeInfo.reset();
    return {};
}

static inline Result<void> storeLastArgument
(
    StoreArgsInfo& storeInfo,
    /*out*/ TokenIterator& iterator, 
    /*out*/ MetaData& metaData, 
    /*out*/ FuncInfo& funcInfo, 
    /*out*/ Nesting& scope
)
{
    string& token = iterator.currentToken;
    if (storeInfo.type.isValid())
    {
        Result<void> result = storeArgument(iterator, storeInfo, funcInfo, scope);
        if (result.hasError)
            return result.error;
    }

    TypeInfo& type = storeInfo.type;
    if(!iterator.nextToken())
        return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

    if(token != ":") 
        return ErrorInfo("function Declarations doesn't end with ':'", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("function Declarations arguments incomplete", iterator.currentLine);

    Result<TypeInfo> returnType = getTypeInfo(iterator, metaData.classStore);
    if (returnType.hasError)
        return returnType.error;

    funcInfo.returnType = returnType.value();
    return {};
}

Result<void> storeArguments(TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, Nesting& scope)
{
    StoreArgsInfo storeInfo;
    uint32_t& openBracketCounter = storeInfo.openBracketCounter;

    string& token = iterator.currentToken;
    while(iterator.nextToken())
    {
        Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
        if(token == ",")
        {
            Result<void> result = storeArgument(iterator, storeInfo, funcInfo, scope);
            if (result.hasError)
                return result.error;
        }
        else if (token == "(")
        {
            openBracketCounter++;
        }
        else if(token == ")")
        {
            if(openBracketCounter <= 1)
            {
                Result<void> result = storeLastArgument(storeInfo, iterator, metaData, funcInfo, scope);
                if (result.hasError)
                    return result.error;

                return {};
            }

            openBracketCounter--;
        }
        else if(getArgType_symbol(token) != ArgumentType::invalid)
        {
            storeInfo.argType = getArgType_symbol(token);
        }
        else if(token == "=")
        {
            storeInfo.argType = argType_ChangeToOptional(storeInfo.argType);

            if (iterator.nextToken())
                break;
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
