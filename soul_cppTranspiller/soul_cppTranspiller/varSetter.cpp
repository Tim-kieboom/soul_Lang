#include "varSetter.h"
#include <sstream>

#include "scope.h"
#include "callFunc.h"
#include "soulCheckers.h"

using namespace std;
static const initializer_list<const char*> allOparations = { "+", "-", "/", "*", "=" };
static const initializer_list<const char*> allConditionOparations = { "<", ">", "==", "!=" };

#define ERROR_VarSetter ErrorInfo("var setter incomplete", iterator.currentLine);

static inline Result<void*> convertBoolSetter(/*out*/stringstream& ss, /*out*/string& token, /*out*/TokenIterator& iterator, MetaData& metaData, const FuncInfo& funcInfo)
{
	string leftCondition;
	if (!iterator.peekToken(/*out*/leftCondition, /*step:*/-1))
		return ERROR_VarSetter;

	Result<VarInfo> leftVarResult = getVarFromScope(funcInfo, metaData, leftCondition);
	DuckType leftType = getDuckType_fromValue(leftCondition);

	ss << token;
	if (!iterator.nextToken(/*out*/token))
		return ERROR_VarSetter;

	if (!leftVarResult.hasError)
	{
		VarInfo leftVar = leftVarResult.value();
		if (!checkValue(token, getDuckType(leftVar.type)))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else if (leftType != DuckType::invalid)
	{
		if (!checkValue(token, leftType))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else
	{
		return ErrorInfo("left condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}

	ss << token;
	return {};
}

static inline Result<bool> endOfVarSetter(TokenIterator& iterator, stringstream& ss, uint32_t openBracketStack, const varSetter_Option& option)
{
	string endOp;
	if (!iterator.peekToken(/*out*/endOp))
		return ERROR_VarSetter;

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
			return ERROR_VarSetter;

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
			ss << ";\n";
			if (!iterator.skipToken())
				return ERROR_VarSetter;

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

Result<string> convertVarSetter(TokenIterator& iterator, MetaData& metaData, const Type& type, FuncInfo& funcInfo, const varSetter_Option& option)
{
	stringstream ss;

	string token;
	if (!iterator.nextToken(token, /*step:*/0))
		return ERROR_VarSetter;

	uint32_t openBracketStack = 0;

	while(true)
	{
		if (token == "(")
		{
			openBracketStack++;
			ss << '(';
		}
		else if (token == ")")
		{
			openBracketStack--;
			ss << ')';
		}
		else
		{
			break;
		}

		if (!iterator.nextToken(/*out*/token))
			return ERROR_VarSetter;
	}

	Result<VarInfo> varResult = getVarFromScope(funcInfo, metaData, token);

	FuncInfo callFunc;
	if(metaData.TryGetfuncInfo(token, callFunc))
	{
		if (getDuckType(type) != getDuckType(callFunc.returnType))
			return ErrorInfo("incompatible type: " + string(toString(type)) + " to returnType of func: \'" + token + ": " + toString(callFunc.returnType) + "\'", iterator.currentLine);
		
		Result<string> callResult = convertFuncCall(iterator, metaData, funcInfo.scope, callFunc, funcInfo);
		if (callResult.hasError)
			return callResult.error;

		ss << callResult.value();
		if (!iterator.nextToken(/*out*/token, /*steps:*/0))
			return ERROR_VarSetter;
	}
	else
	{
		TypeCategory category = getTypeCategory(type);
		if
			(
				!(type == Type::bool_ && token == "!") &&
				!((category == TypeCategory::unsignedInterger || category == TypeCategory::interger) && initListEquals({ "++", "--" }, token))
			)
		{
			if(varResult.hasError && !checkValue(token, getDuckType(type)))
			{
				return (varResult.hasError)
					? ErrorInfo("token: \'" + token + "\' is not valid as a variable", iterator.currentLine)
					: ErrorInfo("token: \'" + token + "\' is not valid for type: " + toString(type), iterator.currentLine);
			}
		}
		ss << token;
	}


	while(iterator.nextToken(/*out*/token))
	{
		Result<VarInfo> varResult = getVarFromScope(funcInfo, metaData, token);
		
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
		else if(token == ";")
		{
			ss << ";\n";
			return ss.str();
		}
		else if(!varResult.hasError)
		{
			ss << token;
		}
		else if(initListEquals(allOparations, token))
		{
			ss << token;
		}
		else if(checkValue(token, type))
		{
			ss << token;
		}
		else if(type == Type::bool_ && initListEquals(allConditionOparations, token))
		{
			Result<void*> result = convertBoolSetter(/*out*/ss, /*out*/token, /*out*/iterator, metaData, funcInfo);
			if (result.hasError)
				return result.error;
		}
		else if(metaData.checkC_str(token))
		{
			if (getDuckType(type) != DuckType::text)
				return ErrorInfo("can not use c_str for type: \'" + toString(type) + '\'', iterator.currentLine);

			ss << token;
		}
		else
		{
			return ErrorInfo("variable oparation invalid invalidToken: \'" + token + "\'", iterator.currentLine);
		}

		Result<bool> isEndResult = endOfVarSetter(iterator, ss, openBracketStack, option);
		if (isEndResult.hasError)
			return isEndResult.error;

		if (isEndResult.value() == true)
			return ss.str();
	}	

	return ERROR_VarSetter;
}