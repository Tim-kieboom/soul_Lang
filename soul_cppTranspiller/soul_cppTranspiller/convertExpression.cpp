#include "convertExpression.h"
#include "Literal.h"
#include "Variable.h"
#include "Assignment.h"
#include "soulChecker.h"
#include "Increment.h"
#include "EmptyExpression.h"
#include "convertFunctionCall.h"
using namespace std;

template <typename T>
class Stack
{
private:
    vector<T> vec;

public:
    Stack() = default;

    bool empty()
    {
        return vec.empty();
    }

    bool topEquals(T value)
    {
        return !vec.empty() && vec.back() == value;
    }

    void push(T value)
    {
        vec.push_back(value);
    }

    bool tryPeek(T& value)
    {
        if (vec.empty())
            return false;

        value = vec.back();
        return true;
    }

    T& peek()
    {
        return vec.back();
    }

    T pop()
    {


        T value = vec.back();
        vec.pop_back();
        return value;
    }
};

static const shared_ptr<EmptyExpression> emptyExpression = make_shared<EmptyExpression>(EmptyExpression());

static inline ErrorInfo ERROR_convertExpression_outOfBounds(TokenIterator& iterator)
{
    return ErrorInfo("unexpected end while parsing BinaryExpression", iterator.currentLine);
}

static inline bool isVariable(Result<VarInfo*>& varResult)
{
    return !varResult.hasError;
}
static inline bool isLiteral(Result<RawType>& LiteralType)
{
    return !LiteralType.hasError;
}
static inline bool isOperator(const string& token)
{
    return getSyntax_Operator(token) != SyntaxTree_Operator::Invalid;
}

static inline Result<void> makeAndPush_BinairyExpression(Stack<shared_ptr<SuperExpression>>& nodeStack, Stack<string>& SymboolStack, int64_t currentLine, SyntaxTree_Operator& opType)
{
    SyntaxTree_Operator oparator = getSyntax_Operator(SymboolStack.pop());
    auto right = nodeStack.pop();
    if (nodeStack.empty())
        return ErrorInfo("BinairyEpression invalid at: \'" + right->printToString() + toString(oparator) + "\'", currentLine);

    auto left = nodeStack.pop();

    if (right->getId() == SyntaxNodeId::EmptyExpresion || left->getId() == SyntaxNodeId::EmptyExpresion)
        return ErrorInfo("EmptyExpression can not go into a BinairyExpression", currentLine);

    nodeStack.push
    (
        make_shared<BinaryExpression>(BinaryExpression(left, oparator, right))
    );

    return {};
}

static inline Result<shared_ptr<Increment>> _convertBeforeVarIncrement(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType* type)
{
    string& token = iterator.currentToken;

    string incrementToken = token;
    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
    if (varResult.hasError)
        return ErrorInfo("token after \'" + incrementToken + "\' has to be a variable", iterator.currentLine);

    VarInfo* var = varResult.value();
    Result<RawType> varType = getRawType_fromStringedRawType(var->stringedRawType, metaData.classStore, iterator.currentLine);
    if (varType.hasError)
        return varType.error;

    *type = varType.value();

    Result<RawType> typeResult = getRawType_fromStringedRawType((*var).stringedRawType, metaData.classStore, iterator.currentLine);
    if (typeResult.hasError)
        return typeResult.error;

    if (getDuckType(typeResult.value()) != DuckType::number)
        return ErrorInfo("variable: \'" + (*var).name + "\' has to be of DuckType::number to use de/increment", iterator.currentLine);

    return make_shared<Increment>
        (
            Increment(make_shared<Variable>(Variable(var->name)), true, (token == "--"), 1)
        );
}

static inline Result<BodyStatment_Result<SuperExpression>> _convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, initializer_list<const char*> endTokens, RawType* shouldBeType, RawType* isType, bool shouldBeMutable)
{
    if (!iterator.nextToken(/*steps:*/-1))
        return ERROR_convertExpression_outOfBounds(iterator);

    BodyStatment_Result<SuperExpression> bodyResult;

    string& token = iterator.currentToken;
    Stack<shared_ptr<SuperExpression>> nodeStack;
    Stack<string> SymboolStack;
    int64_t openBrackets = 0;

    while (iterator.nextToken())
    {
        Result<RawType> literalType = getRawType_fromLiteralValue(token, iterator.currentLine);
        Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

        if (token == "(")
        {
            openBrackets++;
            SymboolStack.push(token);
        }
        else if (token == ")" && --openBrackets >= 0)
        {
            while (!SymboolStack.topEquals("("))
            {
                SyntaxTree_Operator opType;
                Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, SymboolStack, iterator.currentLine, opType);
                if (result.hasError)
                    return result.error;

                if(isType != nullptr && isOperator_booleanOp(opType))
                    *isType = RawType("bool", false);
            }

            if (SymboolStack.topEquals("("))
                SymboolStack.pop();
        }
        else if (initListEquals(endTokens, token))
        {
            break;
        }
        else if (initListEquals({ "++", "--" }, token))
        {
            RawType type;
            string symbool = token;
            Result<shared_ptr<Increment>> incrementResult = _convertBeforeVarIncrement(iterator, metaData, context, &type);
            if (incrementResult.hasError)
                return incrementResult.error;

            shared_ptr<Increment> increment = incrementResult.value();

            if (isType != nullptr)
                *isType = type;

            nodeStack.push(increment);
        }
        else if (isOperator(token))
        {
            while (!SymboolStack.empty() && getOperator_Priority(SymboolStack.peek()) >= getOperator_Priority(token))
            {
                SyntaxTree_Operator opType;
                Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, SymboolStack, iterator.currentLine, opType);
                if (result.hasError)
                    return result.error;

                if (isType != nullptr && isOperator_booleanOp(opType))
                    *isType = RawType("bool", false);
            }

            SymboolStack.push(token);
        }
        else if (metaData.isFunction(token))
        {
            Result<BodyStatment_Result<FunctionCall>> funcCallResult = convertFunctionCall(iterator, metaData, context, token);
            if (funcCallResult.hasError)
                return funcCallResult.error;

            bodyResult.addToBodyResult(funcCallResult.value());
            shared_ptr<FunctionCall> funcCall = funcCallResult.value().expression;

            if (isType != nullptr)
            {
                Result<RawType> type = getRawType_fromStringedRawType(funcCall->getReturnType(), metaData.classStore, iterator.currentLine);
                if (type.hasError)
                    return type.error;

                *isType = type.value();
            }

            nodeStack.push(funcCall);
        }
        else
        {
            if (isVariable(varResult))
            {
                Result<RawType> varType = getRawType_fromStringedRawType(varResult.value()->stringedRawType, metaData.classStore, iterator.currentLine);
                if (varType.hasError)
                    return varType.error;

                bool canMutate = varType.value().isMutable || !varResult.value()->isAssigned;
                if (shouldBeMutable && !canMutate)
                    return ErrorInfo("can not change a const value, var: \'" + varResult.value()->name + "\', type: \'" + varResult.value()->stringedRawType + "\'", iterator.currentLine);

                if (isType != nullptr)
                    *isType = varType.value();

                if (shouldBeType != nullptr)
                {
                    Result<void> isCompatible = varType.value().areTypeCompatible(*shouldBeType, metaData.classStore, iterator.currentLine);
                    if (isCompatible.hasError)
                        return isCompatible.error;
                }

                auto variable = make_shared<Variable>(Variable(token));

                string nextToken;
                if (!iterator.peekToken(nextToken))
                    return ERROR_convertExpression_outOfBounds(iterator);

                if (initListEquals({ "++", "--" }, nextToken))
                {
                    if (!iterator.nextToken())
                        return ERROR_convertExpression_outOfBounds(iterator);

                    nodeStack.push(make_shared<Increment>(Increment(variable, false, (nextToken == "--"), 1)));
                }
                else
                {
                    nodeStack.push(variable);
                }
            }
            else if (isLiteral(literalType))
            {
                if (literalType.value().toDuckType() == DuckType::invalid)
                    return ErrorInfo("Literal has to be one of the primitiveTypes", iterator.currentLine);

                if (isType != nullptr)
                    *isType = literalType.value();

                nodeStack.push(make_shared<Literal>(token));
            }
            else
            {
                return ErrorInfo("token: \'" + token + "\' is not allowed as an Expression", iterator.currentLine);
            }
        }
    }

    while (!SymboolStack.empty())
    {
        SyntaxTree_Operator opType;
        Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, SymboolStack, iterator.currentLine, opType);
        if (result.hasError)
            return result.error;

        if (isType != nullptr && isOperator_booleanOp(opType))
            *isType = RawType("bool", false);
    }

    if (nodeStack.empty())
    {
        bodyResult.expression = dynamic_pointer_cast<SuperExpression>(emptyExpression);
        return bodyResult;
    }

    bodyResult.expression = nodeStack.pop();
    return bodyResult;
}

Result<BodyStatment_Result<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, initializer_list<const char*> endTokens, bool shouldBeMutable, RawType* isType)
{
    return _convertExpression(iterator, metaData, context, endTokens, nullptr, isType, shouldBeMutable);
}

Result<BodyStatment_Result<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& shouldBeType, initializer_list<const char*> endTokens, bool shouldBeMutable, RawType* isType)
{
    return _convertExpression(iterator, metaData, context, endTokens, &shouldBeType, isType, shouldBeMutable);
}
