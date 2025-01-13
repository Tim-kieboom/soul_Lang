#include "convertBody.h"
#include "Increment.h"
#include "Assignment.h"
#include "stringTools.h"
#include "convertAssignment.h"
#include "convertInitVariable.h"
#include "convertFunctionCall.h"
#include "FunctionCallStatment.h"
#include "convertReturnStatment.h"
using namespace std;

static inline ErrorInfo ERROR_convertBody_outOfBounds(FuncDeclaration& funcInfo, TokenIterator& iterator)
{
	return ErrorInfo("unexpected en in functionBody, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);
}

static inline bool isType(Result<RawType>& typeResult)
{
	return !typeResult.hasError;
}

static inline bool isVariable(Result<VarInfo>& varResult)
{
	return !varResult.hasError;
}

static inline bool hasMissingReturnStatment(const std::string& functionName, const RawType& returnType, bool hasReturnStatment)
{
	return functionName != "main" && returnType.toPrimitiveType() != PrimitiveType::none && !hasReturnStatment;
}

static inline Result<shared_ptr<Assignment>> _convertBeforeVarIncrement(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
	string& token = iterator.currentToken;

	string incrementToken = token;
	if (!iterator.nextToken())
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	Result<VarInfo> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (varResult.hasError)
		return ErrorInfo("token after \'" + incrementToken + "\' has to be a variable", iterator.currentLine);

	VarInfo& var = varResult.value();

	Result<RawType> typeResult = getRawType_fromStringedRawType(var.stringedRawType, metaData.classStore, iterator.currentLine);
	if (typeResult.hasError)
		return typeResult.error;

	if (getDuckType(typeResult.value()) != DuckType::number)
		return ErrorInfo("variable: \'" + var.name + "\' has to be of DuckType::number to use de/increment", iterator.currentLine);

	Increment increment = Increment(true, (token == "--"), 1);

	return make_shared<Assignment>
		(
			Assignment(var.name, make_shared<Increment>(increment))
		);
}

Result<FuncNode> convertBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
	shared_ptr<BodyNode> body = make_unique<BodyNode>();

	string checkBracket;
	if (!iterator.peekToken(checkBracket))
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (checkBracket != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);

	string& token = iterator.currentToken;

	Result<RawType> returnTypeResult = getRawType_fromStringedRawType(funcInfo.returnType, metaData.classStore, iterator.currentLine);
	if (returnTypeResult.hasError)
		return returnTypeResult.error;

	RawType& returnType = returnTypeResult.value();

	bool hasReturnStatment = false;
	Result<string> result;
	uint32_t openCurlyBracketCounter = 0;
	while (iterator.nextToken())
	{
		if (iterator.currentLine == 21)
			int f = 0;

		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}
		else if (token == "}")
		{
			openCurlyBracketCounter--;

			if (openCurlyBracketCounter != 0)
				continue;

			if (hasMissingReturnStatment(funcInfo.functionName, returnType, hasReturnStatment))
				return ErrorInfo("Function needs to return something", iterator.currentLine);

			return FuncNode(funcInfo, move(body));
		}

		Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
		Result<VarInfo> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

		if (initListEquals({ "++", "--" }, token))
		{
			Result<shared_ptr<Assignment>> incrementResult = _convertBeforeVarIncrement(iterator, metaData, funcInfo, context);
			if (incrementResult.hasError)
				return incrementResult.error;

			body->addStatment(incrementResult.value());
		}
		else if(isType(typeResult))
		{
			Result<vector<shared_ptr<SuperStatement>>> assignResult = convertInitVariable(iterator, metaData, typeResult.value(), context);
			if (assignResult.hasError)
				return assignResult.error;

			body->addStatment(assignResult.value());
		}
		else if(isVariable(varResult))
		{
			Result<shared_ptr<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
			if (assignResult.hasError)
				return assignResult.error;

			body->addStatment(assignResult.value());
		}
		else if(metaData.isFunction(token))
		{
			Result<shared_ptr<FunctionCall>> funcResult = convertFunctionCall(iterator, metaData, context, token);
			if (funcResult.hasError)
				return funcResult.error;

			body->addStatment
			(
				make_shared<FunctionCallStatment>(FunctionCallStatment(funcResult.value()))
			);
		}
		else if(token == "return")
		{
			Result<shared_ptr<ReturnStatment>> returnResult = convertReturnStatment(iterator, metaData, context, returnType);
			if (returnResult.hasError)
				return returnResult.error;

			body->addStatment(returnResult.value());
			hasReturnStatment = true;
		}
	}

	return ERROR_convertBody_outOfBounds(funcInfo, iterator);
}
