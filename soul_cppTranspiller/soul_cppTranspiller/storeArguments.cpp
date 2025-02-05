#include "storeArguments.h"
#include "soulChecker.h"
#include "convertExpression.h"

using namespace std;

struct StoreArgsInfo
{
    ArgumentType argType = ArgumentType::default_;
    uint32_t openBracketCounter = 1;
    bool isOptional = false;
    shared_ptr<SuperExpression> defaultValue;
    string argName = "";
    RawType type;

    uint64_t argumentPosition = 1;

    void reset(bool isOptional)
    {
        this->type;
        this->argName = "";
        
        if(!isOptional)
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
    vector<ArgumentInfo>& args,
    vector<ArgumentInfo>& optionals
)
{
    RawType& type = storeInfo.type;
    string& argName = storeInfo.argName;
    ArgumentType& argType = storeInfo.argType;
    uint64_t& argumentPosition = storeInfo.argumentPosition;

    if (argName.empty())
        return ErrorInfo("no name given to argument, argumentNumber: \'" + argumentPosition + string("\',") + "argument type : \'" + toString(type) + "\'", iterator.currentLine);

    if (!type.isValid(metaData.classStore))
        return ErrorInfo("valueType of argument is invalid, argument: \'" + argName + "\'", iterator.currentLine);

    if (type.isRefrence())
        return ErrorInfo("Argument type can not be a refrence, argument: \'" + argName + "\'", iterator.currentLine);

    if (argType == ArgumentType::default_)
        type.isMutable = false;

    ArgumentInfo arg = ArgumentInfo(storeInfo.isOptional, type, argName, argType, argumentPosition);

    if (arg.isOptional)
    {
        arg.defaultValue = storeInfo.defaultValue;
        optionals.push_back(arg);
    }
    else
    {
        args.push_back(arg);
    }
    storeInfo.reset(arg.isOptional);
    return {};
}

static inline Result<RawType> storeLastArgument
(
    TokenIterator& iterator,
    StoreArgsInfo& storeInfo,
    MetaData& metaData,
    vector<ArgumentInfo>& args,
    vector<ArgumentInfo>& optionals,
    bool isCtor
)
{
    string& token = iterator.currentToken;
    if (storeInfo.type.isValid(metaData.classStore))
    {
        Result<void> result = storeArgument(iterator, storeInfo, metaData, args, optionals);
        if (result.hasError)
            return result.error;
    }

    RawType& type = storeInfo.type;
    if (isCtor)
        return type;

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

Result<StoreArguments_Result> storeArguments(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& returnType, ClassArgumentInfo* inClass)
{
    StoreArgsInfo storeInfo;
    uint32_t& openBracketCounter = storeInfo.openBracketCounter;
    vector<ArgumentInfo> args;
    vector<ArgumentInfo> optionals;

    string& token = iterator.currentToken;
    while(iterator.nextToken())
    {
        Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
        if (token == ",")
        {
            Result<void> result = storeArgument(iterator, storeInfo, metaData, args, optionals);
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
                Result<RawType> result = storeLastArgument(iterator, storeInfo, metaData, args, optionals, (inClass != nullptr && inClass->isCtor));
                if (result.hasError)
                    return result.error;

                if (inClass == nullptr || !inClass->isCtor)
                    returnType = result.value();
                
                return StoreArguments_Result(args, optionals);
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

            if (!iterator.nextToken())
                break;

            RawType type;
            Result<BodyStatment_Result<SuperExpression>> expression = convertExpression(iterator, metaData, context, { ",", ")" }, false, &type);
            if (expression.hasError)
                return expression.error;

            if (!iterator.nextToken(/*steps*/ -1))
                break;

            Result<void> areCompatible = storeInfo.type.areTypeCompatible(type, metaData.classStore, iterator.currentLine);
            if (areCompatible.hasError)
                return ErrorInfo("optional defaultValue:\n" + areCompatible.error.message, areCompatible.error.lineNumber);

            storeInfo.defaultValue = expression.value().expression;
        }
        else if (!typeResult.hasError)
        {
            storeInfo.type = typeResult.value();
        }
        else if(token == "this")
        {
            ClassInfo classInfo;

            if (inClass == nullptr || !inClass->isCtor)
                return ErrorInfo("'this' is only allowed as argument in ctor methodes", iterator.currentLine);
            
            if(inClass != nullptr) 
                // i HATE compiler 'nooo you can't deref inClass it could be nullptr'
                classInfo = inClass->classInfo;

            if (!iterator.nextToken())
                break;

            if(token != ".")
                return ErrorInfo("'this' allown is an invalid argument (valid argument 'this.<Field-name>')", iterator.currentLine);

            if (!iterator.nextToken())
                break;

            string fieldName = token;

            Result<FieldsInfo> field = classInfo.isField(token, iterator.currentLine);
            if (field.hasError)
                return ErrorInfo("\'" + token + "\' is not a field so can not be used as 'this." + fieldName + "\'", iterator.currentLine);

            Result<RawType> type = getRawType_fromStringedRawType(field.value().stringRawType, metaData.classStore, iterator.currentLine);
            if (type.hasError)
                return type.error;

            if (!iterator.nextToken())
                break;
            
            if (!initListEquals({",", ")"}, token))
                return ErrorInfo("\'"+token+"\' allowed in argument after 'this." + fieldName + "\'", iterator.currentLine);

            storeInfo.argName = "this." + token;
            storeInfo.argType = ArgumentType::mut;
            storeInfo.type = type.value();

            Result<void> result = storeArgument(iterator, storeInfo, metaData, args, optionals);
            if (result.hasError)
                return result.error;

            if (token == ")")
                return StoreArguments_Result(args, optionals);
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
