#include "convertWhileStatment.h"
#include "convertExpression.h"
#include "convertBody.h"
using namespace std;

static inline Result<BodyStatment_Result<SuperExpression>> _convertCondition(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
    string& token = iterator.currentToken;
    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing if statment", iterator.currentLine);

    if (token != "(")
        return ErrorInfo("invalid token after 'if', token; \'" + token + "\'", iterator.currentLine);

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing if statment", iterator.currentLine);

    RawType type;
    Result<BodyStatment_Result<SuperExpression>> expression = convertExpression(iterator, metaData, context, { ")" }, false, &type);
    if (expression.hasError)
        return expression.error;

    if (type.toDuckType() != DuckType::number)
        return ErrorInfo("condition of of statment has to be of DuckType 'number'", iterator.currentLine);

    return expression.value();
}

static inline Result<shared_ptr<BodyNode>> _convertStatmentBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
    ScopeIterator& scope = context.scope;
    uint64_t statementNestingIndex = scope.scope.size();
    scope.scope.emplace_back
    (
        Nesting::makeChild(&scope.getCurrentNesting())
    );

    string nextToken;
    if (!iterator.peekToken(nextToken))
        nextToken = "";

    CurrentContext ifContext = CurrentContext(ScopeIterator(scope.scope, statementNestingIndex));

    if (nextToken != "{")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing if statment", iterator.currentLine);

        Result<BodyStatment_Result<SuperStatement>> statmentResult = convertBodyElement(iterator, metaData, funcInfo, ifContext, SyntaxNodeId::WhileStatment);
        if (statmentResult.hasError)
            return statmentResult.error;

        BodyStatment_Result<SuperStatement>& statment = statmentResult.value();

        auto body = make_shared<BodyNode>(BodyNode());
        for (auto& before : statment.beforeStatment)
            body->addStatment(before);

        body->addStatment(statment.expression);

        for (auto& after : statment.afterStatment)
            body->addStatment(after);

        return body;
    }

    Result<shared_ptr<BodyNode>> ifBodyResult = convertBody(iterator, metaData, funcInfo, ifContext, SyntaxNodeId::WhileStatment);
    if (ifBodyResult.hasError)
        return ifBodyResult.error;

    return ifBodyResult.value();
}

Result<BodyStatment_Result<SuperConditionalStatment>> convertWhileStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
    Result<BodyStatment_Result<SuperExpression>> conditionResult = _convertCondition(iterator, metaData, funcInfo, context);
    if (conditionResult.hasError)
        return conditionResult.error;

    Result<shared_ptr<BodyNode>> ifBodyResult = _convertStatmentBody(iterator, metaData, funcInfo, context);
    if (ifBodyResult.hasError)
        return ifBodyResult.error;

    auto body = BodyStatment_Result<SuperConditionalStatment>
        (
            make_shared<WhileStatment>(WhileStatment(conditionResult.value().expression, ifBodyResult.value()))
        );

    body.addToBodyResult(conditionResult.value());
    return body;
}
