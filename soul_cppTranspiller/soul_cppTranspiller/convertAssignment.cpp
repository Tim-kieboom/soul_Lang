#include "convertAssignment.h"
#include <initializer_list>

#include "stringTools.h"
#include "Increment.h"
#include "BinairyExpression.h"
#include "Variable.h"
#include "Literal.h"
#include "soulChecker.h"
#include "convertExpression.h"

using namespace std;

static const initializer_list<const char*> allOparations = { "+", "-", "/", "*", "&", "|", "&" };
static const initializer_list<const char*> allConditionOparations = { "<", ">", "<=", ">=", "==", "!=", "||", "&&" };

static inline ErrorInfo ERROR_convertAssignment_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("unexpected end while converting Assingment", iterator.currentLine);
}

static inline Result<void> _isSymboolAllowed(string& symbool, MetaData& metaData, VarInfo& varInfo, RawType& type, const TokenIterator& iterator)
{
	static const initializer_list<const char*> allowed_ArraySymbols = { "[", "=" };
	static const initializer_list<const char*> allowed_PointerSymbols = { ".", "=" };
	static const initializer_list<const char*> allowed_DefaultSymbols = { "+=", "-=", "/=", "*=", "&=", "|=", "^=", "&=", "=", "++", "--", "." };
	static const initializer_list<const char*> allowed_ConstSymbols = { ".", "[" };

	initializer_list<const char*> allowedSymbols;

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

	if (!type.isMutable && !initListEquals(allowed_ConstSymbols, symbool))
		return ErrorInfo("can not change a const value, var: \'" + varInfo.name + "\', type: \'" + varInfo.stringedRawType + "\'", iterator.currentLine);

	if (!initListEquals(allowedSymbols, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (metaData.isClass(type) && symbool != "=")
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	return {};
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
	return initListEquals(allConditionOparations, token) || initListEquals(allOparations, token);
}

static inline Result<void> skipEmptyBrackets(TokenIterator& iterator, int64_t& openBracketStack)
{
	string& token = iterator.currentToken;

	while (true)
	{
		if (token == "(")
		{
			openBracketStack++;
		}
		else if (token == ")")
		{
			if (openBracketStack <= 0)
				return ErrorInfo("')' without '('", iterator.currentLine);

			openBracketStack--;
		}
		else
		{
			break;
		}

		if (!iterator.nextToken())
			return ERROR_convertAssignment_outOfBounds(iterator);
	}

	return {};
}

static inline shared_ptr<SuperExpression> addCompountAssignment(const std::string& symbool, const std::string& varName, shared_ptr<SuperExpression>& right)
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

Result<shared_ptr<Assignment>> convertAssignment(TokenIterator& iterator, MetaData& metaData, VarInfo& varInfo, CurrentContext& context)
{
	Result<RawType> typeResult = getRawType_fromStringedRawType(varInfo.stringedRawType, metaData.classStore, iterator.currentLine);
	if (typeResult.hasError)
		return typeResult.error;

	RawType assignVar_type = typeResult.value();

	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ERROR_convertAssignment_outOfBounds(iterator);

	string symbool = token;
	Result<void> isAllowed = _isSymboolAllowed(symbool, metaData, varInfo, assignVar_type, iterator);
	if (isAllowed.hasError)
		return isAllowed.error;

	if (symbool == "[")
	{
		//indexer
		throw exception("not yet implemented");
	}

	if (!iterator.nextToken())
		return ERROR_convertAssignment_outOfBounds(iterator);

	if (initListEquals({ "++", "--" }, symbool))
	{
		Result<PrimitiveType, ClassInfo> trueType;
		if (!assignVar_type.getType(metaData.classStore, trueType))
			return ErrorInfo("variable type is invalid", iterator.currentLine);

		bool isClass = trueType.hasError;

		if (!isClass && getDuckType(trueType.value()) != DuckType::number)
			return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

		if (token != ";")
			return ErrorInfo("variable incomplete atfer \'" + symbool + "\' need ';' but has: " + token, iterator.currentLine);

		return make_shared<Assignment>
			(
				Assignment(varInfo.name, make_shared<Increment>(Increment(false, (symbool == "--"), 1)))
			);
	}

	Result<shared_ptr<SuperExpression>> expressionResult = convertExpression(iterator, metaData, context, assignVar_type, { ";" });
	if (expressionResult.hasError)
		return ErrorInfo("in assingment of Variable: \'" +varInfo.name+ "\'\n" + expressionResult.error.message, expressionResult.error.lineNumber);

	shared_ptr<SuperExpression> expression = addCompountAssignment(symbool, varInfo.name, expressionResult.value());

	return make_shared<Assignment>
		(
			Assignment(varInfo.name, expression)
		);
}
