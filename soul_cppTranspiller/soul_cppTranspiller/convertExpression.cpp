#include "convertExpression.h"
#include "soulChecker.h"
#include "Literal.h"
#include "Variable.h"
#include "EmptyExpression.h"
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

static inline ErrorInfo ERROR_convertExpression_outOfBounds(TokenIterator& iterator)
{
    return ErrorInfo("unexpected end while parsing BinaryExpression", iterator.currentLine);
}

static inline bool isVariable(Result<VarInfo>& varResult)
{
    return !varResult.hasError;
}
static inline bool isLiteral(const string& token, RawType& assignVar_Type, MetaData& metaData)
{
    return checkValue(token, assignVar_Type, metaData);
}
static inline bool isOperator(const string& token)
{
    return getSyntax_Operator(token) != SyntaxTree_Operator::Invalid;
}

static inline void makeAndPush_BinairyExpression(Stack<shared_ptr<SuperExpression>>& nodeStack, Stack<string>& SymboolStack)
{
    SyntaxTree_Operator oparator = getSyntax_Operator(SymboolStack.pop());
    auto right = nodeStack.pop();
    auto left = nodeStack.pop();
    nodeStack.push
    (
        make_shared<BinaryExpression>(BinaryExpression(left, oparator, right))
    );
}

Result<shared_ptr<SuperExpression>> convertExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& shouldBeType, initializer_list<const char*> endTokens)
{
    if (!iterator.nextToken(/*steps:*/-1))
        return ERROR_convertExpression_outOfBounds(iterator);

    string& token = iterator.currentToken;
    Stack<shared_ptr<SuperExpression>> nodeStack;
    Stack<string> SymboolStack;
    int64_t openBrackets = 0;

    while(iterator.nextToken())
    {
        Result<VarInfo> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

        if(token == "(")
        {
            openBrackets++;
            SymboolStack.push(token);
        }
        else if(token == ")" && --openBrackets >= 0)
        {
            while(!SymboolStack.topEquals("("))
                makeAndPush_BinairyExpression(/*out*/nodeStack, SymboolStack);

            if (SymboolStack.topEquals("("))
                SymboolStack.pop();
        }
        else if (initListEquals(endTokens, token))
        {
            break;
        }
        else if(isOperator(token))
        {
            while(!SymboolStack.empty() && getOperator_Priority(SymboolStack.peek()) >= getOperator_Priority(token))
                makeAndPush_BinairyExpression(/*out*/nodeStack, SymboolStack);

            SymboolStack.push(token);
        }
        else
        {
            if (isVariable(varResult))
            {
                Result<RawType> varType = getRawType_fromStringedRawType(varResult.value().stringedRawType, metaData.classStore, iterator.currentLine);
                if (varType.hasError)
                    return varType.error;

                Result<void> isCompatible = varType.value().areTypeCompatible(shouldBeType, metaData.classStore, iterator.currentLine);
                if (isCompatible.hasError)
                    return isCompatible.error;

                nodeStack.push(make_shared<Variable>(Variable(token)));
            }
            else if(isLiteral(token, shouldBeType, metaData))
            {
                nodeStack.push(make_shared<Literal>(token));
            }
            else
            {
                return ErrorInfo("token: \'" + token + "\' is not allowed in this BinairyExpression", iterator.currentLine);
            }
        }
    }

    while (!SymboolStack.empty())
        makeAndPush_BinairyExpression(nodeStack, SymboolStack);

    if (nodeStack.empty())
        return dynamic_pointer_cast<SuperExpression>(make_shared<EmptyExpression>(EmptyExpression()));

    return nodeStack.pop();
}
