#include "convertIndexArray.h"
#include "Literal.h"
#include "Variable.h"
#include "EmptyExpression.h"
#include "RangeExpression.h"
#include "convertExpression.h"
using namespace std;

static inline ErrorInfo ERROR_IndexArray_outOfRange(TokenIterator& iterator)
{
    return ErrorInfo("unexpected end while parsing IndexArray", iterator.currentLine);
}

static inline bool isLiteral(Result<RawType>& literalType)
{
    return !literalType.hasError;
}

static inline bool isVariable(Result<VarInfo*>& varResult)
{
    return !varResult.hasError;
}

static inline bool isRange_WithoutBegin(TokenIterator& iterator)
{
    return iterator.currentToken == "..";
}

static inline bool isRange_WithBegin(TokenIterator& iterator)
{
    string nextToken;
    if (!iterator.peekToken(nextToken))
        return false;

    return nextToken == "..";
}

static inline Result<BodyStatment_Result<SuperExpression>> getIndex(TokenIterator& iterator, MetaData& metaData, CurrentContext& context)
{
    RawType type;
    Result<BodyStatment_Result<SuperExpression>> index = convertExpression(iterator, metaData, context, { "..", "]" }, false, &type);
    if (index.hasError)
        return index.error;

    if (type.toDuckType() != DuckType::Number)
        return ErrorInfo("index has to be of DuckType::number, type is: \'" + toString(type) + "\'", iterator.currentLine);

    if (!iterator.nextToken(-1))
        return ERROR_IndexArray_outOfRange(iterator);

    return index;
}

Result<BodyStatment_Result<IndexArray>> convertIndexArray(TokenIterator& iterator, shared_ptr<SuperExpression>& arrayExpression, MetaData& metaData, CurrentContext& context)
{
    static const shared_ptr<SuperExpression> emptyExpression = make_shared<EmptyExpression>(EmptyExpression());
    BodyStatment_Result<IndexArray> bodyResult;

    string& token = iterator.currentToken;

    if(token != "[")
        return ErrorInfo("\'" +token+ "\' should be '['", iterator.currentLine);

    if (!iterator.nextToken())
        return ERROR_IndexArray_outOfRange(iterator);

    shared_ptr<SuperExpression> index;

    if (isRange_WithBegin(iterator))
    {
        Result<BodyStatment_Result<SuperExpression>> begin = getIndex(iterator, metaData, context);
        if (begin.hasError)
            return begin.error;

        bodyResult.addToBodyResult(begin.value());

        if (!iterator.nextToken(/*steps:*/2))
            return ERROR_IndexArray_outOfRange(iterator);

        if (token == "]")
        {
            index = make_shared<RangeExpression>(RangeExpression(begin.value().expression, emptyExpression));
            if (!iterator.nextToken(/*steps:*/-1))
                return ERROR_IndexArray_outOfRange(iterator);
        }
        else
        {
            Result<BodyStatment_Result<SuperExpression>> end = getIndex(iterator, metaData, context);
            if (end.hasError)
                return end.error;

            bodyResult.addToBodyResult(end.value());
            index = make_shared<RangeExpression>(RangeExpression(begin.value().expression, end.value().expression));
        }
    }
    else if (isRange_WithoutBegin(iterator))
    {
        if (!iterator.nextToken())
            return ERROR_IndexArray_outOfRange(iterator);

        if (token == "]")
            return ErrorInfo("'..' is an invalid rangeExpression for indexing", iterator.currentLine);

        Result<BodyStatment_Result<SuperExpression>> end = getIndex(iterator, metaData, context);
        if (end.hasError)
            return end.error;

        bodyResult.addToBodyResult(end.value());
        index = make_shared<RangeExpression>(RangeExpression(emptyExpression, end.value().expression));
    }
    else
    {
        Result<BodyStatment_Result<SuperExpression>> indexResult = getIndex(iterator, metaData, context);
        if (indexResult.hasError)
            return indexResult.error;

        bodyResult.addToBodyResult(indexResult.value());
        index = indexResult.value().expression;
    }

    if (!iterator.nextToken())
        return ERROR_IndexArray_outOfRange(iterator);

    if (token != "]")
        return ErrorInfo("\'" + token + "\' should be ']'", iterator.currentLine);

    bodyResult.expression = make_shared<IndexArray>(IndexArray(arrayExpression, index));
    return bodyResult;
}
