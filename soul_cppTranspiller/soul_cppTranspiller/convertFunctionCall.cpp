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

static inline void AddOutInitVariableExpression(const string& varName, RawType& type, vector<shared_ptr<ISyntaxNode>>& functionCallExpression)
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

            Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
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

static inline Result<vector<shared_ptr<SuperExpression>>> getExpressionVector(GetArgs_Result& argsResult, FuncDeclaration& funcInfo)
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

static inline Result<BodyStatment_Result<FunctionCall>> _convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName, RawType* shouldBeType = nullptr)
{
    BodyStatment_Result<FunctionCall> bodyResult;
    string& token = iterator.currentToken;

    if (!metaData.isFunction(funcName))
        return ErrorInfo("\'" + funcName + "\' is not a function", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

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
    if (!metaData.tryGetFuncInfo(funcName, args.args, args.optionals, /*out*/funcInfo, iterator.currentLine, error))
        return ErrorInfo("functionCall: \'" + funcName + "\' not found with given arguments\n" + error.message, iterator.currentLine);

    if(shouldBeType != nullptr && shouldBeType->toPrimitiveType() != PrimitiveType::none)
    {
        Result<void> isCompatible = shouldBeType->areTypeCompatible(funcInfo.returnType, metaData.classStore, iterator.currentLine);
        if (isCompatible.hasError)
            return isCompatible.error;
    }

    Result<vector<shared_ptr<SuperExpression>>> expressions = getExpressionVector(args, funcInfo);
    if (expressions.hasError)
        return expressions.error;

    bodyResult.expression =
        make_shared<FunctionCall>
        (
            FunctionCall(funcName, funcInfo.returnType, funcInfo, expressions.value())
        );



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
