#include "convertAssignment.h"
#include <initializer_list>

#include "Literal.h"
#include "Variable.h"
#include "Increment.h"
#include "IndexArray.h"
#include "stringTools.h"
#include "soulChecker.h"
#include "BinairyExpression.h"
#include "convertIndexArray.h"
#include "convertExpression.h"
#include "convertMemberExpression.h"

using namespace std;

static const initializer_list<const char*> allOparations = { "+", "-", "/", "*", "&", "|", "&" };
static const initializer_list<const char*> allConditionOparations = { "<", ">", "<=", ">=", "==", "!=", "||", "&&" };

static inline ErrorInfo ERROR_convertAssignment_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("unexpected end while converting Assingment", iterator.currentLine);
}

static inline Result<void> _isSymboolAllowed(string& symbool, MetaData& metaData, VarInfo* varInfo, RawType type, const TokenIterator& iterator)
{
	static const initializer_list<const char*> allowed_ArraySymbols = { "[", "=" };
	static const initializer_list<const char*> allowed_PointerSymbols = { ".", "=" };
	static const initializer_list<const char*> allowed_DefaultSymbols = { "+=", "-=", "/=", "*=", "&=", "|=", "^=", "&=", "=", "++", "--", "." };
	static const initializer_list<const char*> allowed_ConstSymbols = { ".", "[" };

	initializer_list<const char*> allowedSymbols;

	if (type.isRefrence())
	{
		Result<void> child = type.removeRefrences(iterator.currentLine);
		if (child.hasError)
			return child.error;
	}

	if (type.isArray())
	{
		allowedSymbols = allowed_ArraySymbols;
	}
	else if (type.isPointer())
	{
		allowedSymbols = allowed_PointerSymbols;
	}
	else
	{
		allowedSymbols = allowed_DefaultSymbols;
	}

	bool canMutate = type.isMutable || !varInfo->isAssigned;
	if (!canMutate && !initListEquals(allowed_ConstSymbols, symbool))
		return ErrorInfo("can not change a const value, var: \'" + varInfo->name + "\', type: \'" + varInfo->stringedRawType + "\'", iterator.currentLine);

	if (!initListEquals(allowedSymbols, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (metaData.isClass(type) && symbool != "=")
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	return {};
}

static inline shared_ptr<SuperExpression> _addCompountAssignment(const std::string& symbool, const std::string& varName, shared_ptr<SuperExpression>& right)
{
	shared_ptr<SuperExpression> expression = right;
	shared_ptr<SuperExpression> left = make_shared<Variable>(varName);

	if (symbool == "+=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::Add, right));
	else if (symbool == "-=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::Sub, right));
	else if (symbool == "/=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::Div, right));
	else if (symbool == "*=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::Mul, right));
	else if (symbool == "&=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::BitWise_AND, right));
	else if (symbool == "|=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::BitWise_OR, right));
	else if (symbool == "^=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::BitWise_XOR, right));
	else if (symbool == "&=")
		expression = make_shared<BinaryExpression>(BinaryExpression(left, SyntaxTree_Operator::Modulo, right));

	return expression;
}

Result<BodyStatment_Result<Assignment>> convertAssignment(TokenIterator& iterator, std::shared_ptr<SuperExpression> setVariable, RawType& setVariableType, MetaData& metaData, VarInfo* varInfo, CurrentContext& context)
{
	BodyStatment_Result<Assignment> bodyResult;
	string& token = iterator.currentToken;
	string symbool = token;

	Result<void> isAllowed = _isSymboolAllowed(symbool, metaData, varInfo, setVariableType, iterator);
	if (isAllowed.hasError)
		return isAllowed.error;

	if (!iterator.nextToken())
		return ERROR_convertAssignment_outOfBounds(iterator);

	if (initListEquals({ "++", "--" }, symbool))
	{
		varInfo->isAssigned = true;

		if (setVariableType.toDuckType() != DuckType::Number)
			return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

		if (token != ";")
			return ErrorInfo("variable incomplete atfer \'" + symbool + "\' need ';' but has: " + token, iterator.currentLine);

		auto increment = make_shared<Increment>( Increment(setVariable, false, (symbool == "--"), 1) );
		BodyStatment_Result<Assignment> bodyResult
		(
			make_shared<Assignment>(Assignment(setVariable, increment))
		);

		return bodyResult;
	}

	RawType type;
	bool shouldBeMutable = (setVariableType.isMutable && setVariableType.isRefrence());
	Result<BodyStatment_Result<SuperExpression>> expressionResult = convertExpression(iterator, metaData, context, setVariableType, {";"}, shouldBeMutable, &type);
	if (expressionResult.hasError)
		return ErrorInfo("in assingment of Variable: \'" +varInfo->name+ "\'\n" + expressionResult.error.message, expressionResult.error.lineNumber);

	varInfo->isAssigned = true;

	if (setVariableType.isArray() || setVariableType.isRefrence() || setVariableType.isPointer())
	{
		if (!type.isMutable && setVariableType.isMutable)
			return ErrorInfo("argument: \'" + toString(type) + "\' and argument: \'" + toString(setVariableType) + "\' have diffrent mutability", iterator.currentLine);
	}

	shared_ptr<SuperExpression> expression = _addCompountAssignment(symbool, varInfo->name, expressionResult.value().expression);

	bodyResult.expression = make_shared<Assignment>(Assignment(setVariable, expression));

	bodyResult.addToBodyResult(expressionResult.value());
	return bodyResult;
}
