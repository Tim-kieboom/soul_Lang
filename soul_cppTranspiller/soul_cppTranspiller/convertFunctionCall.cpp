#include "convertFunctionCall.h"
#include "convertExpression.h"

using namespace std;

static inline Result<shared_ptr<FunctionCall>> _convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, FuncDeclaration& funcInfo)
{
    string& token = iterator.currentToken;
    vector<shared_ptr<SuperExpression>> args;
    int64_t argPosition = 0;

    while (token != ")")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);


        if (argPosition+1 > funcInfo.args.size())
            return ErrorInfo("to many args", iterator.currentLine);

        RawType argType = funcInfo.args.at(argPosition++).valueType;
        Result<shared_ptr<SuperExpression>> expression = convertExpression(iterator, metaData, context, argType, { ",", ")" });
        if (expression.hasError)
            return expression.error;

        args.push_back(expression.value());
    }

    return make_shared<FunctionCall>(FunctionCall(funcInfo.functionName, funcInfo.returnType, args));
}

Result<shared_ptr<FunctionCall>> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, const std::string funcName, RawType& shouldBeType)
{
    std::string& token = iterator.currentToken;

    if (!metaData.isFunction(funcName))
        return ErrorInfo("\'" + funcName + "\' is not a function", iterator.currentLine);

    vector<FuncDeclaration> funcInfos = metaData.funcStore[funcName];

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

    if (token != "(")
        return ErrorInfo("FunctionCall: \'"+funcName+"\' had to start with '('", iterator.currentLine);

    for(FuncDeclaration& funcInfo : funcInfos)
    {
        Result<shared_ptr<FunctionCall>> funcResult = _convertFunctionCall(iterator, metaData, context, funcInfo);
        if (!funcResult.hasError)
            return funcResult.value();
    }

    return ErrorInfo("functionCall: \'" + funcName + "\' not found with given arguments", iterator.currentLine);
}
