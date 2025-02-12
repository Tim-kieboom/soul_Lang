#include "convertFunctionCall.h"
#include "Refrence.h"
#include "soulChecker.h"
#include "EmptyStatment.h"
#include "convertExpression.h"
#include "convertInitVariable.h"

using namespace std;

static const shared_ptr<SuperStatement> emptyStatment = make_shared<EmptyStatment>(EmptyStatment());

struct GetArgs_Result
{
    vector<ArgumentInfo> args;
    vector<ArgumentInfo> optionals;
    vector<pair<bool, shared_ptr<SuperExpression>>> expressions;

    GetArgs_Result() = default;
    void push(RawType& type, int64_t argPosition, const std::string& argName, ArgumentType argType, shared_ptr<SuperExpression>& expression, bool isOptional)
    {
        expressions.emplace_back(isOptional, expression);

        if (isOptional)
        {
            optionals.emplace_back
            (
                ArgumentInfo(isOptional, type, argName, argType, argPosition)
            );
        }
        else
        {
            args.emplace_back
            (
                ArgumentInfo(isOptional, type, argName, argType, argPosition)
            );
        }
    }
};

static inline bool isType(Result<RawType>& typeResult)
{
    return !typeResult.hasError;
}

static inline void _AddOutInitVariableExpression(const string& varName, RawType& type, vector<shared_ptr<ISyntaxNode>>& functionCallExpression)
{
    functionCallExpression.push_back
    (
        make_shared<InitializeVariable>(InitializeVariable(toString(type), varName, emptyStatment))
    );
}

static inline Result<GetArgs_Result> _getArgs(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string& funcName, BodyStatment_Result<FunctionCall>& bodyResult)
{
    string& token = iterator.currentToken;
    GetArgs_Result args;
    int64_t argPosition = 1;

    string nextToken;
    if (!iterator.peekToken(nextToken))
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

    if (nextToken == ")")
        return args;

    while (token != ")")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
       
        bool isOptional = false;
        string argName = "<unknown>";
        ArgumentType argType = ArgumentType::default_;
        
        if (!iterator.peekToken(/*out*/nextToken))
            nextToken = "<empty>";

        if(checkName(token) && nextToken == ":")
        {
            argName = token;
            if (!iterator.nextToken(/*steps:*/2))
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            isOptional = true;
        }

        if(token == "out")
        {
            if (!iterator.nextToken())
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            Result<RawType> typeResult = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
            bool hasType = isType(typeResult);

            if (!iterator.peekToken(nextToken))
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            if (hasType && !initListEquals({ ",", ")" }, nextToken))
            {
                string typeString = toString(typeResult.value());

                VarInfo var = VarInfo(nextToken, typeString);
                context.scope.getCurrentNesting().addVariable(var);

                bodyResult.beforeStatment.push_back
                (
                    make_shared<InitializeVariable>(InitializeVariable(typeString, nextToken, emptyStatment))
                );

                if (!iterator.nextToken())
                    return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
            }

            argType = ArgumentType::out;
        }

        RawType expressionType;
        Result<BodyStatment_Result<SuperExpression>> expressionResult = convertExpression(iterator, metaData, context, { ",", ")" }, false, /*isType:*/&expressionType);
        if (expressionResult.hasError)
            return expressionResult.error;
        
        bodyResult.addToBodyResult(expressionResult.value());

        shared_ptr<SuperExpression> expression = expressionResult.value().expression;
        if (argType == ArgumentType::out)
            expression = make_shared<Refrence>(Refrence(expression));

        args.push(expressionType, argPosition, argName, argType, expression, isOptional);
        if(!isOptional)
            argPosition++;
    }

    return args;
}

static inline Result<vector<shared_ptr<SuperExpression>>> _getExpressionsOfArgs(GetArgs_Result& argsResult, FuncDeclaration& funcInfo)
{
    constexpr bool isOptional = true;

    vector<shared_ptr<SuperExpression>> normalExpressions;
    normalExpressions.reserve(funcInfo.args.size() + funcInfo.optionals.size());
    vector<shared_ptr<SuperExpression>> optionalExpressions;
    optionalExpressions.resize(funcInfo.optionals.size());

    for(const auto& kv : funcInfo.optionals)
    {
        const ArgumentInfo& arg = kv.second;
        optionalExpressions.at(arg.argPosition) = arg.defaultValue;
    }

    ArgumentInfo arg;
    uint64_t argsI = 0;
    uint64_t optionalsI = 0;
    for(uint64_t i = 0; i < argsResult.expressions.size(); i++)
    {
        pair<bool, shared_ptr<SuperExpression>>& expression = argsResult.expressions.at(i);

        if(expression.first == isOptional)
        {
            arg = argsResult.optionals.at(optionalsI++);
            uint64_t& argPostion = funcInfo.optionals[arg.argName].argPosition;
            optionalExpressions.at(argPostion) = expression.second;
        }
        else
        {
            normalExpressions.push_back(expression.second);
        }
    }

    for (shared_ptr<SuperExpression>& optional : optionalExpressions)
        normalExpressions.push_back(optional);

    return normalExpressions;
}

static inline Result<void> _templateTypeToType(MetaData& metaData, CurrentContext& context, shared_ptr<FunctionCall>& funcCall, std::map<string, TemplateType_ToType>& templateTypeDefines, GetArgs_Result& called_args, const uint32_t currentLine)
{
    if (funcCall->funcInfo.templateTypes.size() == 0)
        return {};

    FuncDeclaration& funcInfo = funcCall->funcInfo;

    uint32_t i = 0;
    for (ArgumentInfo& calledArg : called_args.args)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = funcArg.valueType.getType_WithoutWrapper();

        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
        {
            funcArg.valueType = templateTypeDefines[rawFuncType].type;

            map<string, TemplateType> dummyTemplate;
            dummyTemplate[rawFuncType] = TemplateType(rawFuncType);
            
            Result<void> isEqual = funcArg.valueType.areTypeCompatible(calledArg.valueType, metaData.classStore, dummyTemplate, currentLine);
            if (isEqual.hasError)
                return isEqual.error;
        }
    }

    i = 0;
    for (ArgumentInfo& calledArg : called_args.optionals)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = calledArg.valueType.getType_WithoutWrapper();

        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
        {
            funcArg.valueType = templateTypeDefines[rawFuncType].type;

            map<string, TemplateType> dummyTemplate;
            dummyTemplate[rawFuncType] = TemplateType(rawFuncType);

            Result<void> isEqual = funcArg.valueType.isEqual(calledArg.valueType, metaData.classStore, dummyTemplate, currentLine, true);
            if (isEqual.hasError)
                return isEqual.error;
        }
    }

    Result<vector<string>> typeTokensResult = getTypeTokens(funcInfo.returnType, currentLine);
    if (typeTokensResult.hasError)
        return typeTokensResult.error;

    vector<string>& typeTokens = typeTokensResult.value();
    if (typeTokens.empty())
        return ErrorInfo("returnType invalid", currentLine);

    uint8_t rawReturnTypeIndex = (typeTokens.at(0) == "const") ? 1 : 0;
    string rawReturnType = typeTokens.at(rawReturnTypeIndex);


    if (templateTypeDefines.find(rawReturnType) != templateTypeDefines.end())
    {
        RawType newReturnType = templateTypeDefines[rawReturnType].type;
        
        for (uint32_t i = rawReturnTypeIndex+1; i < typeTokens.size(); i++) 
        {
            Result<void> result = newReturnType.addTypeWrapper(getTypeWrapper(typeTokens[i]), currentLine);
            if (result.hasError)
                return result.error;
        }

        if (typeTokens.at(0) == "const")
            newReturnType.isMutable = false;

        funcInfo.returnType = toString(newReturnType);
        funcCall->returnType = toString(newReturnType);
    }

    return {};
}

static inline Result<void> _getTemplateDefines_FromCallArgs(std::map<string, TemplateType_ToType>& templateTypeDefines, shared_ptr<FunctionCall> funcCall, GetArgs_Result& called_args, const uint32_t currentLine)
{
    if (templateTypeDefines.size() == funcCall->funcInfo.templateTypes.size())
        return {};

    FuncDeclaration& funcInfo = funcCall->funcInfo;

    uint32_t i = 0;
    for (ArgumentInfo& calledArg : called_args.args)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = funcArg.valueType.getType_WithoutWrapper();
        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
            continue;

         if(funcInfo.templateTypes.find(rawFuncType) != funcInfo.templateTypes.end() && templateTypeDefines.find(rawFuncType) == templateTypeDefines.end())
            templateTypeDefines[rawFuncType] = TemplateType_ToType(funcInfo.templateTypes[rawFuncType], calledArg.valueType);
    }

    i = 0;
    for (ArgumentInfo& calledArg : called_args.optionals)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = calledArg.valueType.getType_WithoutWrapper();
        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
            continue;

        if (funcInfo.templateTypes.find(rawFuncType) != funcInfo.templateTypes.end() && templateTypeDefines.find(rawFuncType) == templateTypeDefines.end())
            templateTypeDefines[rawFuncType] = TemplateType_ToType(funcInfo.templateTypes[rawFuncType], calledArg.valueType);
    }

    return {};
}

static inline Result<void> _convertAndCheck_TemplateTypes(MetaData& metaData, CurrentContext& context, shared_ptr<FunctionCall> funcCall, GetArgs_Result& called_args, vector<RawType>& definedTemplateTypes, const uint32_t currentLine)
{
    std::map<string, TemplateType_ToType>& templateTypeDefines = funcCall->templateTypeDefines;
    
    uint32_t i = 0;
    for(auto& kv : funcCall->funcInfo.templateTypes)
    {
        if (i + 1 > definedTemplateTypes.size())
            break;

        templateTypeDefines[kv.first] = TemplateType_ToType(kv.second, definedTemplateTypes.at(i++));
    }
        

    Result<void> result = _getTemplateDefines_FromCallArgs(/*out*/templateTypeDefines, funcCall, called_args, currentLine);
    if (result.hasError)
        return result.error;

    result = _templateTypeToType(metaData, context, /*out*/funcCall, templateTypeDefines, called_args, currentLine);
    if (result.hasError)
        return result.error;

    return {};
}

static inline Result<vector<RawType>> _getDefinedTemplateType(TokenIterator& iterator, MetaData& metaData, CurrentContext& context)
{
    string& token = iterator.currentToken;
    vector<RawType> types;

    while (iterator.nextToken())
    {
        Result<RawType> defineType = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
        if (defineType.hasError)
            return defineType.error;

        types.push_back(defineType.value());
        
        if (!iterator.nextToken())
            break;

        if (token == ",")
        {
            continue;
        }
        else if (token == ">")
        {
            return types;
        }
        else
        {
            return ErrorInfo("\'" + token + "\' is invalid in templateType ctor", iterator.currentLine);
        }
    }

    return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
}

static inline Result<BodyStatment_Result<FunctionCall>> _convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, std::string funcName, RawType* shouldBeType = nullptr)
{
    BodyStatment_Result<FunctionCall> bodyResult;
    string& token = iterator.currentToken;

    if (funcName == "bool")
    {
        funcName = "bool_";
    }
    else if(funcName == "char")
    {
        funcName = "char_";
    }

    if (!metaData.isFunction(funcName, context))
        return ErrorInfo("\'" + funcName + "\' is not a function", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

    vector<RawType> definedTemplateTypes;
    if(token == "<")
    {
        Result<vector<RawType>> TemplateTypesResult = _getDefinedTemplateType(iterator, metaData, context);
        if (TemplateTypesResult.hasError)
            return TemplateTypesResult.error;

        definedTemplateTypes = TemplateTypesResult.value();

        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
    }

    if (token != "(")
        return ErrorInfo("FunctionCall: \'" + funcName + "\' had to start with '('", iterator.currentLine);

    string nextToken;
    if (!iterator.peekToken(nextToken))
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

    GetArgs_Result args;
    if(nextToken == ")")
    {
        args = GetArgs_Result();

        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
    }
    else
    {
        Result<GetArgs_Result> argsResult = _getArgs(iterator, metaData, context, funcName, bodyResult);
        if (argsResult.hasError)
            return argsResult.error;

        args = argsResult.value();
    }

    ErrorInfo error;
    FuncDeclaration funcInfo;
    if (!metaData.tryGetFunction(funcName, context, args.args, args.optionals, /*out*/funcInfo, iterator.currentLine, error))
        return ErrorInfo("functionCall: \'" + funcName + "\' not found with given arguments\n" + error.message, iterator.currentLine);

    Result<vector<shared_ptr<SuperExpression>>> expressionsOfArgs = _getExpressionsOfArgs(args, funcInfo);
    if (expressionsOfArgs.hasError)
        return expressionsOfArgs.error;

    bodyResult.expression =
        make_shared<FunctionCall>
        (
            FunctionCall(funcName, funcInfo.returnType, funcInfo, expressionsOfArgs.value())
        );

    Result<void> result = _convertAndCheck_TemplateTypes(metaData, context, /*out*/bodyResult.expression, args, definedTemplateTypes, iterator.currentLine);
    if (result.hasError)
        return result.error;

    if (shouldBeType != nullptr && shouldBeType->toPrimitiveType() != PrimitiveType::none)
    {
        Result<void> isCompatible = shouldBeType->areTypeCompatible(funcInfo.returnType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
        if (isCompatible.hasError)
            return isCompatible.error;
    }

    return bodyResult;
}

Result<BodyStatment_Result<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName)
{
    return _convertFunctionCall(iterator, metaData, context, funcName, nullptr);
}

Result<BodyStatment_Result<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName, RawType& shouldBeType)
{
    return _convertFunctionCall(iterator, metaData, context, funcName, &shouldBeType);
}
