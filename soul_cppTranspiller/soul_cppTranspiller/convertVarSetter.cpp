#include "convertVarSetter.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "convertIndexer.h"
#include "convertFunctionCall.h"

using namespace std;

static const initializer_list<const char*> allOparations = { "+", "-", "/", "*", "=" };
static const initializer_list<const char*> allConditionOparations = { "<", ">", "==", "!=" };

static inline ErrorInfo ERROR_convertVarSetter_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("var setter incomplete", iterator.currentLine);
}

static inline Result<void> _checkTypeOrVar(const TypeInfo& type, const std::string& token, TokenIterator& iterator, Result<VarInfo>& varResult)
{
	TypeCategory category = getTypeCategory(type.primType);
	if
	(
		!(type.isArray || type.isPointer) &&
		!(type.primType == PrimitiveType::bool_ && initListEquals({ "~", "!" }, token)) &&
		!((category == TypeCategory::unsignedInterger || category == TypeCategory::interger) && initListEquals({ "++", "--" }, token))
	)
	{
		if (varResult.hasError && !checkValue(token, type))
		{
			return (varResult.hasError)
				? ErrorInfo("token: \'" + token + "\' is not valid as a variable", iterator.currentLine)
				: ErrorInfo("token: \'" + token + "\' is not valid for type: " + toString(type), iterator.currentLine);
		}
	}
	return {};
}

static inline Result<void> _convertBoolSetter(stringstream& ss, TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, const FuncInfo& funcInfo)
{
	string leftCondition;
	string& token = iterator.currentToken;
	if (!iterator.peekToken(/*out*/leftCondition, /*steps:*/-1))
		return ERROR_convertVarSetter_outOfBounds(iterator);

	Result<VarInfo> leftVarResult = scope.tryGetVariable_fromCurrent(leftCondition, metaData.globalScope, iterator.currentLine);
	Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
	
	ss << token;
	if (!iterator.nextToken())
		return ERROR_convertVarSetter_outOfBounds(iterator);

	if(!leftVarResult.hasError)
	{
		if(!checkValue(token, leftVarResult.value().type))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else if(!typeResult.hasError)
	{
		if (!checkValue(token, typeResult.value()))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else
	{
		return ErrorInfo("left condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}

	ss << token;
	return {};
}

static inline Result<bool> _endOfVarSetter
(
	TokenIterator& iterator, 
	MetaData& metaData, 
	stringstream& ss, 
	int64_t openBracketStack, 
	const varSetter_Option& option,
	bool addEndl
)
{
	string endOp;
	if (!iterator.peekToken(/*out*/endOp))
		return ERROR_convertVarSetter_outOfBounds(iterator);

	switch (option)
	{
	case varSetter_Option::endComma:
	{
		if (endOp == ",")
		{
			ss << ", ";
			return true;
		}
	}
	break;

	case varSetter_Option::endRoundBracket:
	{
		if (!iterator.peekToken(/*out*/endOp, /*step:*/0))
			return ERROR_convertVarSetter_outOfBounds(iterator);

		if (endOp == ")" && openBracketStack == 0)
		{
			return true;
		}
	}
	break;

	case varSetter_Option::endSemiColon:
	{
		if (endOp == ";")
		{
			ss << ';';
			if (metaData.transpillerOption.addEndLines && addEndl)
				ss << '\n';

			if (!iterator.skipToken())
				return ERROR_convertVarSetter_outOfBounds(iterator);

			return true;
		}
	}
	break;

	case varSetter_Option::endComma_or_RoundBrasket:
	{
		if (endOp == ",")
		{
			ss << ", ";
			return true;
		}

		if (!iterator.peekToken(/*out*/endOp, /*step:*/0))
			return ERROR_convertVarSetter_outOfBounds(iterator);

		if (endOp == ")" && openBracketStack <= 0)
		{
			return true;
		}
	}
	break;

	default:
		return ErrorInfo("incorrect varSetter_Option", iterator.currentLine);
		break;
	}

	return false;
}

static inline Result<void> _goToFirstNonBracketToken(stringstream& ss, TokenIterator& iterator, string& token, int64_t& openBracketStack)
{
	while (true)
	{
		if (token == "(")
		{
			openBracketStack++;
			ss << '(';
		}
		else if (token == ")")
		{
			if (openBracketStack <= 0)
				return ErrorInfo("')' without '('", iterator.currentLine);

			openBracketStack--;
			ss << ')';
		}
		else
		{
			break;
		}

		if (!iterator.nextToken())
			return ERROR_convertVarSetter_outOfBounds(iterator);
	}

	return {};
}

static inline Result<void> _convertNew(stringstream& ss, TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, const TypeInfo& type, const FuncInfo& funcInfo)
{
	string& token = iterator.currentToken;

	if (!(type.isArray || type.isPointer))
		return ErrorInfo("can not use \'new\' while type is not pointer or array type: \'" + toString(type) + "\'", iterator.currentLine);

	if (!iterator.nextToken())
		return ERROR_convertVarSetter_outOfBounds(iterator);

	Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
	if (typeResult.hasError)
		return typeResult.error;

	TypeInfo newType = typeResult.value();
	ss << ' ' << typeToCppType(newType);

	Result<void> isComparable = type.areTypesCompatiple(newType, iterator.currentLine);
	if (isComparable.hasError)
		return isComparable.error;

	if (!iterator.nextToken())
		return ERROR_convertVarSetter_outOfBounds(iterator);

	if (token == "[")
	{
		ss << '[';
		if (!iterator.nextToken())
			return ERROR_convertVarSetter_outOfBounds(iterator);

		if (getDuckType_fromValue(token) != DuckType::number)
			return ErrorInfo("array constructor only takes a number, token: \'" + token + "\'", iterator.currentLine);

		ss << token;

		if (!iterator.nextToken())
			return ERROR_convertVarSetter_outOfBounds(iterator);

		if (token != "]")
			return ErrorInfo("unexpected end of array constructor, token: \'" + token + "\', should be \']\'", iterator.currentLine);

		ss << ']';
	}
	else if (token == "(")
	{
		throw std::exception("not yet implemented");
	}
	else
	{
		return ErrorInfo("token: \'" + token + "\' not a valid constructors", iterator.currentLine);
	}

	return {};
}

static inline Result<void> _checkFirstToken(stringstream& ss, TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, const TypeInfo& type, FuncInfo& funcInfo)
{
	string& token = iterator.currentToken;
	Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

	if (metaData.isFunction(token))
	{
		Result<void> isCompatible = type.areTypesCompatiple(metaData.funcStore[token].front().returnType, iterator.currentLine);
		if (isCompatible.hasError)
			return isCompatible.error;

		Result<string> callResult = convertFunctionCall(iterator, metaData, scope, token, funcInfo);
		if (callResult.hasError)
			return callResult.error;

		ss << callResult.value();
	}
	else if (!type.isComplexType)
	{
		Result<void> result = _checkTypeOrVar(type, token, iterator, varResult);
		if (result.hasError)
			return result.error;

		ss << token;
	}

	if (token == "new")
	{
		if (!iterator.nextToken(/*steps:*/-1))
			return ERROR_convertVarSetter_outOfBounds(iterator);
	}

	return {};
}

Result<string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	const varSetter_Option& option,
	bool addEndl
)
{
	int64_t openBracketStack = 0;
	return convertVarSetter(iterator, metaData, type, funcInfo, scope, option, openBracketStack, addEndl);
}

Result<string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	const varSetter_Option& option,
	int64_t& openBracketStack,
	bool addEndl
)
{
	stringstream ss;
	string& token = iterator.currentToken;

	Result<void> result = _goToFirstNonBracketToken(/*out*/ss, /*out*/iterator, /*out*/token, /*out*/openBracketStack);
	if (result.hasError)
		return result.error;

	result = _checkFirstToken(/*out*/ss, /*out*/iterator, /*out*/metaData, /*out*/scope, type, /*out*/funcInfo);
	if (result.hasError)
		return result.error;

	Result<bool> isEndResult = _endOfVarSetter(iterator, metaData, ss, openBracketStack, option, addEndl);
	if (isEndResult.hasError)
		return isEndResult.error;

	if (isEndResult.value() == true)
		return ss.str();

	while (iterator.nextToken())
	{
		Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

		if (token == "(")
		{
			openBracketStack++;
			ss << token;
		}
		else if (token == ")")
		{
			openBracketStack--;
			ss << token;
		}
		else if (token == "[")
		{
			Result<string> indexResult = convertIndexer(/*out*/iterator, /*out*/funcInfo);
			if (indexResult.hasError)
				return indexResult.error;

			ss << indexResult.value();
		}
		else if (token == ";")
		{
			ss << ';';
			if (metaData.transpillerOption.addEndLines && addEndl)
				ss << '\n';
			return ss.str();
		}
		else if (!varResult.hasError)
		{
			ss << token;
		}
		else if (initListEquals(allOparations, token))
		{
			ss << token;
		}
		else if (checkValue(token, type))
		{
			ss << token;
		}
		else if (token == "new")
		{
			Result<void> result = _convertNew(/*out*/ss, /*out*/iterator, /*out*/metaData, /*out*/scope, type, funcInfo);
			if (result.hasError)
				return result.error;
		}
		else if (type.primType == PrimitiveType::bool_ && initListEquals(allConditionOparations, token))
		{
			Result<void> result = _convertBoolSetter(/*out*/ss, /*out*/iterator, metaData, scope, funcInfo);
			if (result.hasError)
				return result.error;
		}

		Result<bool> isEndResult = _endOfVarSetter(iterator, metaData, ss, openBracketStack, option, addEndl);
		if (isEndResult.hasError)
			return isEndResult.error;

		if (isEndResult.value() == true)
			return ss.str();
	}

	return ERROR_convertVarSetter_outOfBounds(iterator);
}