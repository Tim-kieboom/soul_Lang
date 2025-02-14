#include "convertExpression.h"
#include "Literal.h"
#include "Variable.h"
#include "Increment.h"
#include "Assignment.h"
#include "soulChecker.h"
#include "ConstructArray.h"
#include "CopyExpression.h"
#include "RangeExpression.h"
#include "EmptyExpression.h"
#include "convertIndexArray.h"
#include "convertFunctionCall.h"
using namespace std;

template <typename T>
class Stack
{
private:
    vector<T> vec;

public:
    Stack() = default;

    bool empty() const
    {
        return vec.empty();
    }

    bool topEquals(T value) const
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

    uint64_t size() const
    {
        return vec.size();
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

static inline bool isNegativeSymbool(const string& token, const Stack<shared_ptr<SuperExpression>>& nodeStack, const Stack<string>& symboolStack)
{
    return (token == "-" && (nodeStack.empty() || !symboolStack.empty()));
}

static inline bool isOperator(const string& token)
{
    return getSyntax_Operator(token) != SyntaxTree_Operator::Invalid;
}

static inline Result<RawType> getArrayItemType(const RawType& type, const TokenIterator& iterator)
{
    Result<RawType> arrayItemType = type.getTypeChild(iterator.currentLine);
    if (arrayItemType.hasError)
        return arrayItemType.error;

    Result<void> removedRefs = arrayItemType.value().removeRefrences(iterator.currentLine);
    if (removedRefs.hasError)
        return removedRefs.error;

    Result<void> addRef = arrayItemType.value().addTypeWrapper(TypeWrapper::refrence, iterator.currentLine);
    if (addRef.hasError)
        return addRef.error;

    return arrayItemType;
}

static inline Result<void> makeAndPush_BinairyExpression(Stack<shared_ptr<SuperExpression>>& nodeStack, Stack<string>& SymboolStack, int64_t currentLine, SyntaxTree_Operator& opType)
{
    static shared_ptr<Literal> trueLiteral = make_shared<Literal>(Literal("true"));

    SyntaxTree_Operator oparator = getSyntax_Operator(SymboolStack.pop());
    shared_ptr<SuperExpression> right = nodeStack.pop();
    shared_ptr<SuperExpression> left;

    if (oparator == SyntaxTree_Operator::Not)
    {
        if (right->getId() == SyntaxNodeId::EmptyExpresion)
            return ErrorInfo("EmptyExpression can not go into a BinairyExpression", currentLine);

        nodeStack.push
        (
            make_shared<BinaryExpression>( BinaryExpression(right, SyntaxTree_Operator::NotEquals, trueLiteral) )
        );
    }
    else
    {
        if (nodeStack.empty())
            return ErrorInfo("BinairyEpression invalid at: \'" + right->printToString() + " " + toString(oparator) + "\'", currentLine);

        left = nodeStack.pop();

        if (right->getId() == SyntaxNodeId::EmptyExpresion || left->getId() == SyntaxNodeId::EmptyExpresion)
            return ErrorInfo("EmptyExpression can not go into a BinairyExpression", currentLine);

        nodeStack.push
        (
            make_shared<BinaryExpression>(BinaryExpression(left, oparator, right))
        );
    }

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
    Result<RawType> varType = getRawType_fromStringedRawType(var->stringedRawType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
    if (varType.hasError)
        return varType.error;

    *type = varType.value();

    Result<RawType> typeResult = getRawType_fromStringedRawType((*var).stringedRawType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
    if (typeResult.hasError)
        return typeResult.error;

    if (getDuckType(typeResult.value()) != DuckType::Number)
        return ErrorInfo("variable: \'" + (*var).name + "\' has to be of DuckType::number to use de/increment", iterator.currentLine);

    return make_shared<Increment>
        (
            Increment(make_shared<Variable>(Variable(var->name)), true, (token == "--"), 1)
        );
}

static inline Result<shared_ptr<ConstructArray>> _getConstructArray(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& arrayType)
{
    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    string& token = iterator.currentToken;

    Result<RawType> literalType = getRawType_fromLiteralValue(token, iterator.currentLine);
    Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
    
    RawType ctorArgType;
    shared_ptr<SuperExpression> amountElementsExpression;
    if(isLiteral(literalType))
    {
        ctorArgType = literalType.value();
        amountElementsExpression = make_shared<Literal>(Literal(token));
    }
    else if (isVariable(varResult))
    {
        Result<RawType> varType = getRawType_fromStringedRawType(varResult.value()->stringedRawType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
        if (varType.hasError)
            return varType.error;

        ctorArgType = varType.value();
        amountElementsExpression = make_shared<Variable>(Variable(token));
    }
    else
    {
        return ErrorInfo("\'" + token + "\' is invalid for array Constructor", iterator.currentLine);
    }

    if(ctorArgType.toDuckType() != DuckType::Number)
        return ErrorInfo("\'" + token + "\' invalid type: \'"+ toString(ctorArgType) + "\' for array Constructor", iterator.currentLine);

    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    return make_shared<ConstructArray>(ConstructArray(toString(arrayType), amountElementsExpression));
}

static inline Result<BodyStatment_Result<SuperExpression>> _getVariableExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, Result<VarInfo*> varResult, RawType* shouldBeType, RawType* isType, bool shouldBeMutable)
{
    BodyStatment_Result<SuperExpression> bodyResult;

    string& token = iterator.currentToken;

    Result<RawType> varType = getRawType_fromStringedRawType(varResult.value()->stringedRawType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
    if (varType.hasError)
        return varType.error;

    bool isLiteralStr = varResult.value()->stringedRawType == "const str" && varResult.value()->isCompileConst;
    bool canMutate = varType.value().isMutable || !varResult.value()->isAssigned || isLiteralStr;
    if (shouldBeMutable && !canMutate)
            return ErrorInfo("can not change a const value, var: \'" + varResult.value()->name + "\', type: \'" + varResult.value()->stringedRawType + "\'", iterator.currentLine);

    string nextToken;
    if (!iterator.peekToken(nextToken))
        nextToken = "";

    shared_ptr<SuperExpression> variableExpression;

    if (initListEquals({ "++", "--" }, nextToken))
    {
        auto variable = make_shared<Variable>(Variable(token));

        if (!iterator.nextToken())
            return ERROR_convertExpression_outOfBounds(iterator);

        variableExpression = make_shared<Increment>
            (
                Increment(variable, false, (nextToken == "--"), 1)
            );
    }
    else if (nextToken == "[")
    {
        Result<BodyStatment_Result<IndexArray>> indexResult = convertIndexArray(iterator, metaData, context);
        if (indexResult.hasError)
            return indexResult.error;

        bodyResult.addToBodyResult(indexResult.value());
        variableExpression = indexResult.value().expression;
        
        if (indexResult.value().expression->index->getId() == SyntaxNodeId::RangeExpression)
        {
            if (shouldBeType != nullptr && !varType.value().isMutable && shouldBeType->isMutable)
                return ErrorInfo("argument: \'" + toString(varType.value()) + "\' and argument: \'" + toString(*shouldBeType) + "\' have diffrent mutability", iterator.currentLine);
        }
        else 
        {
            varType = getArrayItemType(varType.value(), iterator);
        }
    }
    else
    {
        variableExpression = make_shared<Variable>(Variable(token));

        if (shouldBeType != nullptr)
        {
            Result<void> isCompatible = varType.value().areTypeCompatible(*shouldBeType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
            if (isCompatible.hasError)
                return isCompatible.error;
        }
    }

    if (isType != nullptr)
        *isType = varType.value();

    bodyResult.expression = variableExpression;
    return bodyResult;
}

static inline Result<shared_ptr<SuperExpression>> _getNewExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType* isType)
{
    string& token = iterator.currentToken;

    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    Result<RawType> newTypeResult = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
    if (newTypeResult.hasError)
        return newTypeResult.error;

    RawType& newType = newTypeResult.value();

    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    shared_ptr<SuperExpression> newExpression;
    if (token == "[")
    {
        Result<shared_ptr<ConstructArray>> newArrayResult = _getConstructArray(iterator, metaData, context, /*out*/newType);
        if (newArrayResult.hasError)
            return newArrayResult.error;

        Result<void> setToArray = newType.addTypeWrapper(TypeWrapper::array_, iterator.currentLine);
        if (setToArray.hasError)
            return setToArray.error;

        newExpression = newArrayResult.value();
    }
    else if (token == "(")
    {
        throw exception("new ctor not yet implemented");
    }
    else
    {
        return ErrorInfo("\'" + token + "\' invalid in 'new' statment", iterator.currentLine);
    }

    if (isType != nullptr)
        *isType = newType;

    return newExpression;
}

static inline Result<void> _setNegativeExpression
(
    TokenIterator& iterator, 
    MetaData& metaData,
    CurrentContext& context, 
    Stack<shared_ptr<SuperExpression>>& nodeStack,
    RawType* shouldBeType,
    RawType* isType,
    BodyStatment_Result<SuperExpression>& bodyResult,
    bool shouldBeMutable
)
{
    static shared_ptr<Literal> minusOne = make_shared<Literal>(Literal("-1"));

    string& token = iterator.currentToken;
    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    string negativeToken = "-" + token;
    Result<RawType> literalType = getRawType_fromLiteralValue(negativeToken, iterator.currentLine);
    Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

    RawType type;
    if (isVariable(varResult))
    {
        Result<BodyStatment_Result<SuperExpression>> varExpression = _getVariableExpression(iterator, metaData, context, varResult, shouldBeType, &type, shouldBeMutable);
        if (varExpression.hasError)
            return varExpression.error;

        bodyResult.addToBodyResult(varExpression.value());

        nodeStack.push
        (
            make_shared<BinaryExpression>(BinaryExpression(varExpression.value().expression, SyntaxTree_Operator::Mul, minusOne))
        );
    }
    else if (isLiteral(literalType))
    {
        if (literalType.value().toDuckType() == DuckType::invalid)
            return ErrorInfo("Literal has to be one of the primitiveTypes", iterator.currentLine);

        type = literalType.value();

        nodeStack.push(make_shared<Literal>(negativeToken));
    }
    else
    {
        return ErrorInfo("\'" + token + "\' is invalid after '-' (can only be Variable of Literal)", iterator.currentLine);
    }

    if (isType != nullptr)
        *isType = type;

    if (type.toDuckType() != DuckType::Number)
        return ErrorInfo("you can only make DuckType::number negative (remove '-' from: \'"+token+"\')", iterator.currentLine);

    return {};
}

static inline Result<void> _getCopyExpression
(
    TokenIterator& iterator,
    MetaData& metaData,
    CurrentContext& context,
    Stack<shared_ptr<SuperExpression>>& nodeStack,
    RawType* shouldBeType,
    RawType* isType,
    BodyStatment_Result<SuperExpression>& bodyResult,
    bool shouldBeMutable
)
{
    string& token = iterator.currentToken;

    if (!iterator.nextToken())
        return ERROR_convertExpression_outOfBounds(iterator);

    shared_ptr<SuperExpression> copyArgument;
    if (token == "[")
    {
        if (!iterator.nextToken())
            return ERROR_convertExpression_outOfBounds(iterator);

        Result<RawType> literalType = getRawType_fromLiteralValue(token, iterator.currentLine);
        Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

        RawType type;
        RawType indexShouldBeType = RawType("f64", true);
        if (isVariable(varResult))
        {
            Result<BodyStatment_Result<SuperExpression>> varExpression = _getVariableExpression(iterator, metaData, context, varResult, &indexShouldBeType, &type, shouldBeMutable);
            if (varExpression.hasError)
                return varExpression.error;

            bodyResult.addToBodyResult(varExpression.value());

            copyArgument = varExpression.value().expression;
        }
        else if (isLiteral(literalType))
        {
            if (literalType.value().toDuckType() == DuckType::invalid)
                return ErrorInfo("Literal has to be one of the primitiveTypes", iterator.currentLine);

            type = literalType.value();

            copyArgument = std::make_shared<Literal>(Literal(token));
        }
        else
        {
            return ErrorInfo("\'" + token + "\' is invalid after '-' (can only be Variable of Literal)", iterator.currentLine);
        }

        if (!iterator.nextToken(/*steps:*/2))
            return ERROR_convertExpression_outOfBounds(iterator);
    }
    else
    {
        copyArgument = emptyExpression;
    }

    RawType type;
    Result<BodyStatment_Result<SuperExpression>> copyExpression = convertExpression(iterator, metaData, context, { ";" }, false, &type);
    if (copyExpression.hasError)
        return copyExpression.error;

    bodyResult.addToBodyResult(copyExpression.value());

    if (type.isPrimitiveType() && (!type.isArray() && !type.isPointer()))
        return ErrorInfo("can not use 'copy' oparator for primitive type that is not an array or pointer", iterator.currentLine);

    type.isMutable = true;

    if (shouldBeType != nullptr)
    {
        Result<void> result = type.isEqual(*shouldBeType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine, shouldBeMutable);
        if (result.hasError)
            return result.error;
    }

    if (isType != nullptr)
        *isType = type;

    nodeStack.push
    (
        make_shared<CopyExpression>(CopyExpression(toString(type), copyExpression.value().expression, copyArgument))
    );

    return {};
}

static inline Result<void> _getAllExpressions
(
    TokenIterator& iterator,
    MetaData& metaData,
    CurrentContext& context,
    Stack<shared_ptr<SuperExpression>>& nodeStack,
    Stack<string>& symboolStack,
    initializer_list<const char*> endTokens, 
    RawType* shouldBeType, 
    BodyStatment_Result<SuperExpression>& bodyResult,
    Stack<RawType>& typeStack,
    bool shouldBeMutable
)
{
    string& token = iterator.currentToken;
    int64_t openBrackets = 0;

    while (iterator.nextToken())
    {
        Result<RawType> literalType = getRawType_fromLiteralValue(token, iterator.currentLine);
        Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

        string nextToken;
        if (!iterator.peekToken(nextToken))
            nextToken = "";

        if (token == "(")
        {
            openBrackets++;
            symboolStack.push(token);
        }
        else if (token == ")" && --openBrackets >= 0)
        {
            while (!symboolStack.topEquals("("))
            {
                SyntaxTree_Operator opType;
                Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, symboolStack, iterator.currentLine, opType);
                if (result.hasError)
                    return result.error;

                if (isOperator_booleanOp(opType))
                    typeStack.push(RawType("bool", true));
            }

            if (symboolStack.topEquals("("))
                symboolStack.pop();
        }
        else if (initListEquals(endTokens, token))
        {
            return {};
        }
        else if (initListEquals({ "++", "--" }, token))
        {
            RawType type;
            string symbool = token;
            Result<shared_ptr<Increment>> incrementResult = _convertBeforeVarIncrement(iterator, metaData, context, &type);
            if (incrementResult.hasError)
                return incrementResult.error;

            shared_ptr<Increment> increment = incrementResult.value();

            typeStack.push(type);
            nodeStack.push(increment);
        }
        else if (isOperator(token))
        {
            uint32_t operatorPrecedence = getOperator_Precedence(token);
            while (!symboolStack.empty() && getOperator_Precedence(symboolStack.peek()) >= operatorPrecedence)
            {
                SyntaxTree_Operator opType;
                Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, symboolStack, iterator.currentLine, opType);
                if (result.hasError)
                    return result.error;

                if (isOperator_booleanOp(opType))
                    typeStack.push(RawType("bool", true));
            }

            if (isNegativeSymbool(token, nodeStack, symboolStack))
            {
                RawType type;
                Result<void> result = _setNegativeExpression(iterator, metaData, context, nodeStack, shouldBeType, &type, bodyResult, shouldBeMutable);
                if (result.hasError)
                    return result.error;

                typeStack.push(type);
                continue;
            }

            symboolStack.push(token);
        }
        else if (token == "copy")
        {
            RawType type;
            Result<void> result = _getCopyExpression(iterator, metaData, context, nodeStack, shouldBeType, &type, bodyResult, shouldBeMutable);
            if (result.hasError)
                return result.error;

            typeStack.push(type);
            return {};
        }
        else if (metaData.isFunction(token, context))
        {
            Result<BodyStatment_Result<FunctionCall>> funcCallResult = convertFunctionCall(iterator, metaData, context, token);
            if (funcCallResult.hasError)
                return funcCallResult.error;

            bodyResult.addToBodyResult(funcCallResult.value());
            shared_ptr<FunctionCall> funcCall = funcCallResult.value().expression;

            Result<RawType> type = getRawType_fromStringedRawType(funcCall->getReturnType(), metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
            if (type.hasError)
                return type.error;

            if (shouldBeType != nullptr)
            {
                Result<void> result = type.value().areTypeCompatible(*shouldBeType, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
                if (result.hasError)
                    return ErrorInfo("functionCall:\'" + funcCall->funcName + "\' returnType: \'" + toString(type.value()) + "\' is not compatible with Type: \'" + toString(*shouldBeType) + "\'", iterator.currentLine);
            }

            typeStack.push(type.value());
            nodeStack.push(funcCall);
        }
        else if (isVariable(varResult))
        {
            if (!varResult.value()->isAssigned)
                return ErrorInfo("\'" + varResult.value()->name + "\' can not be used before it is assigned", iterator.currentLine);

            RawType type;
            Result<BodyStatment_Result<SuperExpression>> varExpression = _getVariableExpression(iterator, metaData, context, varResult, shouldBeType, &type, shouldBeMutable);
            if (varExpression.hasError)
                return varExpression.error;

            bodyResult.addToBodyResult(varExpression.value());
            typeStack.push(type);
            nodeStack.push(varExpression.value().expression);
        }
        else if (isLiteral(literalType))
        {
            if (literalType.value().toDuckType() == DuckType::invalid)
                return ErrorInfo("Literal has to be one of the primitiveTypes", iterator.currentLine);

            typeStack.push(literalType.value());
            nodeStack.push(make_shared<Literal>(token));
        }
        else if (token == "new")
        {

            RawType type;
            Result<shared_ptr<SuperExpression>> newExpression = _getNewExpression(iterator, metaData, context, &type);
            if (newExpression.hasError)
                return newExpression.error;

            typeStack.push(type);
            nodeStack.push(newExpression.value());
        }
        else
        {
            return ErrorInfo("token: \'" + token + "\' is not allowed as an Expression", iterator.currentLine);
        }
    }

    return ERROR_convertExpression_outOfBounds(iterator);
}

static inline Result<BodyStatment_Result<SuperExpression>> _convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, initializer_list<const char*> endTokens, RawType* shouldBeType, RawType* isType, bool shouldBeMutable)
{
    if (!iterator.nextToken(/*steps:*/-1))
        return ERROR_convertExpression_outOfBounds(iterator);

    BodyStatment_Result<SuperExpression> bodyResult;

    int64_t beginI = iterator.i+1;

    string& token = iterator.currentToken;
    Stack<shared_ptr<SuperExpression>> nodeStack;
    Stack<string> symboolStack;
    Stack<RawType> typeStack;

    Result<void> result = _getAllExpressions(iterator, metaData, context, /*out*/nodeStack, /*out*/symboolStack, endTokens, shouldBeType, bodyResult, typeStack, shouldBeMutable);
    if (result.hasError)
        return result.error;

    while (!symboolStack.empty())
    {
        SyntaxTree_Operator opType;
        Result<void> result = makeAndPush_BinairyExpression(/*out*/nodeStack, symboolStack, iterator.currentLine, opType);
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

    if (nodeStack.size() > 1)
    {
        std::stringstream ss;

        for (uint32_t i = (uint32_t)beginI; i < (uint32_t)(iterator.i-1); i++)
            ss << iterator.tokens.at(i).text << ' ';

        ss << iterator.tokens.at(iterator.i-1).text;

        return ErrorInfo("\'" + ss.str() + "\' is invalid, not a valid singleExpression or a valid binairyExpression", iterator.currentLine);
    }

    if (isType != nullptr)
        *isType = typeStack.peek();

    if (context.functionRuleSet == CurrentContext::FuncRuleSet::Functional)
    {
        if (typeStack.peek().isMutable)
            return ErrorInfo("Functional function can not have mutable expressions", iterator.currentLine);
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
