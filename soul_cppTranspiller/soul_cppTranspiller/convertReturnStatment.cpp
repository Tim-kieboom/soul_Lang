#include "convertReturnStatment.h"
#include "convertExpression.h"
#include "EmptyExpression.h"
using namespace std;

BodyStatment_Result<ReturnStatment> emptyReturnResult
(
    make_shared<ReturnStatment>(ReturnStatment(make_shared<EmptyExpression>(EmptyExpression())))
);

Result<BodyStatment_Result<ReturnStatment>> convertReturnStatment(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, FuncDeclaration& funcInfo)
{
    string& token = iterator.currentToken;


    Result<RawType> returnTypeResult = getRawType_fromStringedRawType(funcInfo.returnType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
    if (returnTypeResult.hasError)
        return returnTypeResult.error;

    RawType returnType = returnTypeResult.value();

    if (token != "return")
        return ErrorInfo("ReturnStatment needs to begin with 'return'", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing ReturnStatment", iterator.currentLine);

    if(token == ";")
    {
        if (funcInfo.functionName != "main" && returnType.toPrimitiveType() != PrimitiveType::none)
            return ErrorInfo("'return' can not be empty if returnType: \'" + toString(returnType) + "\'", iterator.currentLine);

        return emptyReturnResult;
    }

    RawType type;
    Result<BodyStatment_Result<SuperExpression>> expression = convertExpression(iterator, metaData, context, { ";" }, false, /*isType:*/&type);
    if (expression.hasError)
        return expression.error;

    if (type.areTypeCompatible(returnType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine).hasError)
        return ErrorInfo("trying to return type: \'" + toString(type) + "\', while return type is returnType: \'" + toString(returnType) + "\'", iterator.currentLine);

    BodyStatment_Result<ReturnStatment> bodyResult
    (
        make_shared<ReturnStatment>(ReturnStatment(expression.value().expression))
    );

    bodyResult.addToBodyResult(expression.value());
    return bodyResult;
}
