#include "convertForStatment.h"
#include "convertExpression.h"
#include "convertAssignment.h"
#include "convertInitVariable.h"
#include "ForStatment.h"
#include "convertBody.h"

using namespace std;

static inline bool isVariable(Result<VarInfo*>& var)
{
    return !var.hasError;
}

static inline bool isType(Result<RawType>& type)
{
    return !type.hasError;
}

static inline Result<shared_ptr<BodyNode>> _convertStatmentBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& forContext)
{
    string nextToken;
    if (!iterator.peekToken(nextToken))
        nextToken = "";


    if (nextToken != "{")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing if statment", iterator.currentLine);

        Result<BodyStatment_Result<SuperStatement>> statmentResult = convertBodyElement(iterator, metaData, funcInfo, forContext);
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

    Result<shared_ptr<BodyNode>> ifBodyResult = convertBody(iterator, metaData, funcInfo, forContext, /*isFuncBody:*/false);
    if (ifBodyResult.hasError)
        return ifBodyResult.error;

    return ifBodyResult.value();
}

Result<BodyStatment_Result<SuperConditionalStatment>> convertForStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
    BodyStatment_Result<SuperConditionalStatment> bodyResult;

    if(!iterator.nextToken(/*steps:*/2))
        return ErrorInfo("unexpected end while parsing 'for'", iterator.currentLine);

    ScopeIterator& scope = context.scope;
    uint64_t statementNestingIndex = scope.scope.size();
    scope.scope.emplace_back
    (
        Nesting::makeChild(&scope.getCurrentNesting())
    );
    CurrentContext forContext = CurrentContext(ScopeIterator(scope.scope, statementNestingIndex));

    string& token = iterator.currentToken;
    Nullable<std::pair<std::shared_ptr<InitializeVariable>, std::shared_ptr<Assignment>>> initStatment;
    if(token != ";")
    {
        Result<RawType> typeResult = getRawType(iterator, metaData.classStore);

        if(isType(typeResult))
        {
            Result<BodyStatment_Result<InitializeVariable>> initResult = convertInitVariable(iterator, metaData, typeResult.value(), forContext);
            if (initResult.hasError)
                return initResult.error;

            BodyStatment_Result<InitializeVariable>& init = initResult.value();

            if (init.afterStatment.size() <= 0)
                return ErrorInfo("first argument of 'for' statment doesn't have an assignment (so add '= <value>')", iterator.currentLine);

            if(init.afterStatment.back()->getId() != SyntaxNodeId::Assignment)
                return ErrorInfo("first argument of 'for' statment doesn't have an assignment (so add '= <value>')", iterator.currentLine);

            if(init.afterStatment.size() > 1 || init.beforeStatment.size() > 0)
                return ErrorInfo("token: \'" + token + "\', invalid in first argument of 'for' statment (MUST init and assign duckType::number value)", iterator.currentLine);


            std::shared_ptr<InitializeVariable> initVar = init.expression;
            std::shared_ptr<Assignment> assign = dynamic_pointer_cast<Assignment>(init.afterStatment.back());
            pair<std::shared_ptr<InitializeVariable>, std::shared_ptr<Assignment>> _pair(initVar, assign);
            initStatment = Nullable<std::pair<std::shared_ptr<InitializeVariable>, std::shared_ptr<Assignment>>>(_pair);
        }
        else
        {
            return ErrorInfo("token: \'" +token+ "\', invalid in first argument of 'for' statment (MUST init and assign duckType::number value)", iterator.currentLine);
        }

    }

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing 'for'", iterator.currentLine);

    Result<BodyStatment_Result<SuperExpression>> stopCondition = convertExpression(iterator, metaData, forContext, { ";" }, false);
    if (stopCondition.hasError)
        return stopCondition.error;

    bodyResult.addToBodyResult(stopCondition.value());

    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while parsing 'for'", iterator.currentLine);

    Result<BodyStatment_Result<SuperExpression>> doAfterEachLoop = convertExpression(iterator, metaData, forContext, { ")" }, false);
    if (doAfterEachLoop.hasError)
        return doAfterEachLoop.error;

    bodyResult.addToBodyResult(doAfterEachLoop.value());

    Result<shared_ptr<BodyNode>> body = _convertStatmentBody(iterator, metaData, funcInfo, forContext);
    if (body.hasError)
        return body.error;

    bodyResult.expression = make_shared<ForStatment>(ForStatment(initStatment, stopCondition.value().expression, doAfterEachLoop.value().expression, body.value()));
    return bodyResult;
}
