#include "convertVarSetter.h"
#include "convertFunctionCall.h"
#include "soulCheckers.h"
#include "cppConverter.h"

using namespace std;

static const initializer_list<const char*> allOparations = { "+", "-", "/", "*", "=" };
static const initializer_list<const char*> allConditionOparations = { "<", ">", "==", "!=" };

static inline ErrorInfo ERROR_convertVarSetter_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("var setter incomplete", iterator.currentLine);
}

static inline Result<void> checkTypeOrVar(const TypeInfo& type, const std::string& token, TokenIterator& iterator, Result<VarInfo>& varResult)
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

static inline Result<void> convertBoolSetter(stringstream& ss, TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, const FuncInfo& funcInfo)
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

static inline Result<void> convertConstructor(stringstream& ss, TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, const FuncInfo& funcInfo)
{

}

static inline Result<bool> endOfVarSetter
(
	TokenIterator& iterator, 
	MetaData& metaData, 
	stringstream& ss, 
	uint32_t openBracketStack, 
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

		if (endOp == ")" && openBracketStack == 0)
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

Result<string> convertVarSetter
(
	TokenIterator& iterator,
	MetaData& metaData,
	const TypeInfo& type,
	FuncInfo& funcInfo,
	ScopeIterator& scope,
	EndVarSetter endVarFunc,
	const varSetter_Option& option,
	bool addEndl
)
{
    stringstream ss;
    string& token = iterator.currentToken;

	uint32_t openBracketStack = 0;

	while (true)
	{
		if (token == "(")
		{
			openBracketStack++;
			ss << '(';
		}
		else if (token == ")")
		{
			if (openBracketStack == 0)
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

	Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

	FuncInfo callFunc;

	if(metaData.TryGetfuncInfo(token, callFunc))
	{
		Result<void> isCompatible = type.areTypesCompatiple(callFunc.returnType, iterator.currentLine);
		if (isCompatible.hasError)
			return isCompatible.error;

		Result<string> callResult = convertFunctionCall(iterator, metaData, scope, callFunc, funcInfo);
		if (callResult.hasError)
			return callResult.error;

		ss << callResult.value();
	}
	else if(!type.isComplexType)
	{
		Result<void> result = checkTypeOrVar(type, token, iterator, varResult);
		if (result.hasError)
			return result.error;

		ss << token;
	}

	if (token == "new")
	{
		if (!iterator.nextToken(/*steps:*/-1))
			return ERROR_convertVarSetter_outOfBounds(iterator);
	}

	while(iterator.nextToken())
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
		else if(token == "new")
		{
			if (!(type.isArray || type.isPointer))
				return ErrorInfo("can not use \'new\' while type is not pointer or array type: \'" + toString(type) + "\'", iterator.currentLine);
			
			if (!iterator.nextToken())
				break;

			Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);
			if (typeResult.hasError)
				return typeResult.error;

			TypeInfo newType = typeResult.value();
			ss << ' ' << typeToCppType(newType);

			Result<void> isComparable = type.areTypesCompatiple(newType, iterator.currentLine);
			if (isComparable.hasError)
				return isComparable.error;

			if (!iterator.nextToken())
				break;

			if(token == "[")
			{
				ss << '[';
				if (!iterator.nextToken())
					break;

				if (getDuckType_fromValue(token) != DuckType::number)
					return ErrorInfo("array constructor only takes a number, token: \'" + token + "\'", iterator.currentLine);
				
				ss << token;

				if (!iterator.nextToken())
					break;

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

		}
		else if (type.primType == PrimitiveType::bool_ && initListEquals(allConditionOparations, token))
		{
			Result<void> result = convertBoolSetter(/*out*/ss, /*out*/iterator, metaData, scope, funcInfo);
			if (result.hasError)
				return result.error;
		}
		else
		{
			return ErrorInfo("variable oparation invalid invalidToken: \'" + token + "\'", iterator.currentLine);
		}

		Result<bool> isEndResult = endOfVarSetter(iterator, metaData, ss, openBracketStack, option, addEndl);
		if (isEndResult.hasError)
			return isEndResult.error;

		if (isEndResult.value() == true)
			return ss.str();
	}

	return ERROR_convertVarSetter_outOfBounds(iterator);
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
	return convertVarSetter(iterator, metaData, type, funcInfo, scope, &endOfVarSetter, option, addEndl);
}