#include "convertReturnStatment.h"
#include "convertExpression.h"
using namespace std;

Result<BodyStatment_Result<ReturnStatment>> convertReturnStatment(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& returnType)
{
    string& token = iterator.currentToken;

    if (token != "return")
        return ErrorInfo("ReturnStatment needs to begin with 'return'", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing ReturnStatment", iterator.currentLine);

    RawType type;
    Result<BodyStatment_Result<SuperExpression>> expression = convertExpression(iterator, metaData, context, { ";" }, false, /*isType:*/&type);
    if (expression.hasError)
        return expression.error;

    if (type.areTypeCompatible(returnType, metaData.classStore, iterator.currentLine).hasError)
        return ErrorInfo("trying to return type: \'" + toString(type) + "\', while return type is returnType: \'" + toString(returnType) + "\'", iterator.currentLine);

    BodyStatment_Result<ReturnStatment> bodyResult
    (
        make_shared<ReturnStatment>(ReturnStatment(expression.value().expression))
    );

    bodyResult.addToBodyResult(expression.value());
    return bodyResult;
}
