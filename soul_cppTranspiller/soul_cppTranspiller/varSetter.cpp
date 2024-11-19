#include "varSetter.h"
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
	Type leftType = getType(leftCondition);

	ss << token;
	if (!iterator.nextToken(/*out*/token))
		return ERROR_VarSetter;

	if (!leftVarResult.hasError)
	{
		VarInfo leftVar = leftVarResult.value();
		if (!checkValue(token, getDuckType(leftVar.type)))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else if (leftType != Type::invalid)
	{
		if (!checkValue(token, getDuckType(leftType)))
			return ErrorInfo("condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}
	else
	{
		return ErrorInfo("left condition invalid, left: \'" + leftCondition + "\', right: \'" + token + '\'', iterator.currentLine);
	}

	ss << token;
	return {};
}

Result<string> convertVarSetter(TokenIterator& iterator, MetaData& metaData, const Type& type, FuncInfo& funcInfo)
{
	stringstream ss;

	string token;
	if (!iterator.nextToken(token, /*step:*/0))
		return ERROR_VarSetter;

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
			if (varResult.hasError && !checkValue(token, type))
			{
				if (!checkValue(token, type))
					return ErrorInfo("token: \'" + token + "\' is not valid for type: " + toString(type), iterator.currentLine);

				if (varResult.hasError)
					return ErrorInfo("token: \'" + token + "\' is not valid as a variable", iterator.currentLine);
			}
		}
		ss << token;
	}


	while(iterator.nextToken(/*out*/token))
	{
		Result<VarInfo> varResult = getVarFromScope(funcInfo, metaData, token);
		if(token == ";")
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

		string endOp;
		if (!iterator.peekToken(/*out*/endOp))
			return ERROR_VarSetter;

		if (endOp == ",")
		{
			return ss.str();
		}
		else if (endOp == ";")
		{
			if (!iterator.skipToken())
				return ERROR_VarSetter;
			ss << ";\n";
			return ss.str();
		}
	}

	return ERROR_VarSetter;
}