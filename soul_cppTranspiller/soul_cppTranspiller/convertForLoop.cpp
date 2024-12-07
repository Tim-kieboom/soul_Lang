#include "convertForLoop.h"
#include "convertVarInit.h"
#include "convertVar.h"

using namespace std;

static inline ErrorInfo ERROR_convertForLoop_outOfBounds(TokenIterator& iterator)
{
    return ErrorInfo("forLoop end enexpected", iterator.currentLine);
}

static inline Result<void> convertForLoop_firstCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
    string& token = iterator.currentToken;
    if (!iterator.nextToken())
        return ERROR_convertForLoop_outOfBounds(iterator);

    if(token == ";")
    {
        ss << ';';
        return {};
    }

    if(token == "const")
        return ErrorInfo("iterator variable of forLoop can not be const", iterator.currentLine);

    Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
    Result<VarInfo> varInfo = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

    Result<string> varResult;
    if(!typeResult.hasError)
    {
        TypeInfo type = typeResult.value();
        if(getDuckType(type.primType) != DuckType::number)
            return ErrorInfo("iterator variable of forLoop has to by of a number type currentType: \'" + toString(type) + "\'", iterator.currentLine);

        FuncInfo _;
        varResult = convertVarInit(type, iterator, metaData, _, funcInfo, scope);
    }
    else if (!varInfo.hasError)
    {
        varResult = convertVar(varInfo.value(), iterator, metaData, funcInfo, scope);
    }
    else
    {
        return ErrorInfo("first forloop condition invalid token: \'" + token + "\'", iterator.currentLine);
    }

    if (varResult.hasError)
        return varResult.error;

    ss << varResult.value();
    return {};
}

static inline Result<void> convertForLoop_secondCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
    string token;

    if (!iterator.nextToken())
        return ERROR_convertForLoop_outOfBounds(iterator);

    if (token == ";")
        return ErrorInfo("second condition of forLoop can't be empty", iterator.currentLine);

    static const TypeInfo boolType = TypeInfo(PrimitiveType::bool_);
    Result<string> varSetterResult = convertVarSetter(iterator, metaData, boolType, funcInfo, scope, varSetter_Option::endSemiColon);
    if (varSetterResult.hasError)
        return varSetterResult.error;

    ss << varSetterResult.value();
    return {};
}

static inline Result<void> convertForLoop_thirdCondition(stringstream& ss, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
    string& token = iterator.currentToken;

    if (!iterator.nextToken())
        return ERROR_convertForLoop_outOfBounds(iterator);

    if (token == ";")
        return {};

    string var = token;
    if (token == "++" || token == "--")
    {
        if (!iterator.peekToken(/*out*/var))
            return ERROR_convertForLoop_outOfBounds(iterator);
    }

    Result<VarInfo> varInfoResult = scope.tryGetVariable_fromCurrent(var, metaData.globalScope, iterator.currentLine);
    if (varInfoResult.hasError)
        return varInfoResult.error;

    Result<string> varResult = convertVar(varInfoResult.value(), iterator, metaData, funcInfo, scope, varSetter_Option::endRoundBracket);
    if (varResult.hasError)
        return varResult.error;

    ss << varResult.value();
    return {};
}

Result<string> convertForLoop(TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope)
{
    stringstream ss;
    string& token = iterator.currentToken;

    if (token != "(")
        return ErrorInfo("forLoop needs to start with \'(\', starts with: \'" + token + "\'", iterator.currentLine);

    ss << '(';

    Result<void> result;
    result = convertForLoop_firstCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
    if (result.hasError)
        return result.error;

    result = convertForLoop_secondCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
    if (result.hasError)
        return result.error;

    result = convertForLoop_thirdCondition(/*out*/ss, iterator, metaData, funcInfo, scope);
    if (result.hasError)
        return result.error;

    return ss.str();
}
