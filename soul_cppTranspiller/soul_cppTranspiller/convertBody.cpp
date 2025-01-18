#include "convertBody.h"
#include "Increment.h"
#include "Assignment.h"
#include "stringTools.h"
#include "soulChecker.h"
#include "EmptyStatment.h"
#include "convertExpression.h"
#include "convertAssignment.h"
#include "convertInitVariable.h"
#include "convertFunctionCall.h"
#include "CompileConstVariable.h"
#include "FunctionCallStatment.h"
#include "convertReturnStatment.h"
#include "ConditionalStatmentsId.h"
#include "SuperConditionalStatment.h"
#include "convertCompileConstVariable.h"
#include "convertIf_else_elseIfStatments.h"
using namespace std;


static inline ErrorInfo ERROR_convertBody_outOfBounds(FuncDeclaration& funcInfo, TokenIterator& iterator)
{
	return ErrorInfo("unexpected en in functionBody, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);
}

static inline bool isType(Result<RawType>& typeResult)
{
	return !typeResult.hasError;
}

static inline bool isVariable(Result<VarInfo*>& varResult)
{
	return !varResult.hasError;
}

static inline bool hasMissingReturnStatment(const std::string& functionName, const RawType& returnType, bool hasReturnStatment)
{
	return functionName != "main" && returnType.toPrimitiveType() != PrimitiveType::none && !hasReturnStatment;
}

static inline Result<BodyStatment_Result<SuperConditionalStatment>> _convertConditionStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, ConditionalStatmentsId conditionStatmentId)
{
	switch(conditionStatmentId)
	{
	case ConditionalStatmentsId::if_:
		return convertIfStatment(iterator, metaData, funcInfo, context);
	case ConditionalStatmentsId::else_:
		return convertElseStatment(iterator, metaData, funcInfo, context);
	case ConditionalStatmentsId::else_if_:
		return convertElseIfStatment(iterator, metaData, funcInfo, context);

	case ConditionalStatmentsId::for_:
		return {};
	case ConditionalStatmentsId::while_:
		return {};

	case ConditionalStatmentsId::switch_:
		return {};

	case ConditionalStatmentsId::invalid:
	default:
		return ErrorInfo("conditional statment type unknown", iterator.currentLine);
	}
}

Result<BodyStatment_Result<SuperStatement>> convertBodyElement(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
	uint32_t openCurlyBracketCounter = 0;
	return convertBodyElement(iterator, metaData, funcInfo, context, openCurlyBracketCounter, false);
}

Result<BodyStatment_Result<SuperStatement>> convertBodyElement(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, uint32_t& openCurlyBracketCounter, bool isFuncBody)
{
	string& token = iterator.currentToken;

	if (token == "{")
	{
		openCurlyBracketCounter++;
		if (!iterator.nextToken())
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);
	}
	else if (token == "}")
	{
		openCurlyBracketCounter--;

		if (openCurlyBracketCounter != 0)
		{
			if (!iterator.nextToken())
				return ERROR_convertBody_outOfBounds(funcInfo, iterator);
		}

		Result<RawType> returnTypeResult = getRawType_fromStringedRawType(funcInfo.returnType, metaData.classStore, iterator.currentLine);
		if (returnTypeResult.hasError)
			return returnTypeResult.error;

		RawType& returnType = returnTypeResult.value();

		if (isFuncBody && hasMissingReturnStatment(funcInfo.functionName, returnType, funcInfo.hasReturnStament))
			return ErrorInfo("Function needs to return something", iterator.currentLine);

		return BodyStatment_Result<SuperStatement>(make_shared<EmptyStatment>(EmptyStatment()));
	}

	Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

	string nextToken;
	if (!iterator.peekToken(nextToken))
		nextToken = "";

	if (initListEquals({ "++", "--" }, token))
	{
		string varName;
		if (!iterator.peekToken(varName))
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);

		varResult = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, iterator.currentLine);
		if (varResult.hasError)
			return varResult.error;

		Result<BodyStatment_Result<SuperExpression>> increment = convertExpression(iterator, metaData, context, { ";" }, true);
		if (increment.hasError)
			return increment.error;

		auto assignment = make_shared<Assignment>(Assignment(varName, increment.value().expression));
		auto res = BodyStatment_Result<SuperStatement>(assignment);
		res.addToBodyResult(increment.value());
		return res;
	}
	else if (isType(typeResult))
	{
		Result<BodyStatment_Result<InitializeVariable>> initResult = convertInitVariable(iterator, metaData, typeResult.value(), context);
		if (initResult.hasError)
			return initResult.error;

		BodyStatment_Result<InitializeVariable>& init = initResult.value();
		return BodyStatment_Result<SuperStatement>(init.expression, init);
	}
	else if (isVariable(varResult))
	{
		Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
		if (assignResult.hasError)
			return assignResult.error;

		BodyStatment_Result<Assignment>& assign = assignResult.value();
		return BodyStatment_Result<SuperStatement>(assign.expression, assign);
	}
	else if (metaData.isFunction(token))
	{
		Result<BodyStatment_Result<FunctionCall>> funcResult = convertFunctionCall(iterator, metaData, context, token);
		if (funcResult.hasError)
			return funcResult.error;

		BodyStatment_Result<FunctionCall>& funcCall = funcResult.value();
		shared_ptr<FunctionCallStatment> funcStatment = make_shared<FunctionCallStatment>(FunctionCallStatment(funcCall.expression));

		if (!iterator.nextToken())
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);

		return BodyStatment_Result<SuperStatement>(funcStatment, funcCall);
	}
	else if (token == "return")
	{
		Result<BodyStatment_Result<ReturnStatment>> returnResult = convertReturnStatment(iterator, metaData, context, funcInfo);
		if (returnResult.hasError)
			return returnResult.error;

		BodyStatment_Result<ReturnStatment>& returnStatment = returnResult.value();

		funcInfo.hasReturnStament = true;
		return BodyStatment_Result<SuperStatement>(returnStatment.expression, returnStatment);
	}
	else if (token == "CompileConst")
	{
		Result<BodyStatment_Result<CompileConstVariable>> compileConst = convertCompileConstVariable(iterator, metaData, context);
		if (compileConst.hasError)
			return compileConst.error;

		return BodyStatment_Result<SuperStatement>(compileConst.value().expression, compileConst.value());
	}
	else if (getConditionStatment(token, nextToken) != ConditionalStatmentsId::invalid)
	{
		ConditionalStatmentsId conditionStatment = getConditionStatment(token, nextToken);

		Result<BodyStatment_Result<SuperConditionalStatment>> conditionalstatmentResult = _convertConditionStatment(iterator, metaData, funcInfo, context, conditionStatment);
		if (conditionalstatmentResult.hasError)
			return conditionalstatmentResult.error;

		BodyStatment_Result<SuperConditionalStatment>& conditionalStatment = conditionalstatmentResult.value();
		return BodyStatment_Result<SuperStatement>(conditionalStatment.expression, conditionalStatment);
	}
	else
	{
		return ErrorInfo("unknown token: \'" + token + "\'", iterator.currentLine);
	}
}

Result<shared_ptr<BodyNode>> convertBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, bool isFuncBody)
{
	static const shared_ptr<BodyNode> emptyBodyNode = make_shared<BodyNode>(BodyNode());
	shared_ptr<BodyNode> body = make_unique<BodyNode>();

	string checkBracket;
	if (!iterator.peekToken(checkBracket))
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (checkBracket != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);

	if (!iterator.peekToken(checkBracket, /*steps:*/2))
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (checkBracket == "}")
	{
		if (!iterator.nextToken(/*steps:*/2))
			return ERROR_convertBody_outOfBounds(funcInfo, iterator);

		return emptyBodyNode;
	}

	string& token = iterator.currentToken;

	bool hasReturnStatment = false;
	Result<string> result;
	uint32_t openCurlyBracketCounter = 0;
	while (iterator.nextToken())
	{
		if (iterator.currentLine == 148)
			int d = 0;

		Result<BodyStatment_Result<SuperStatement>> bodyElementResult = convertBodyElement(iterator, metaData, funcInfo, context, openCurlyBracketCounter, isFuncBody);
		if (bodyElementResult.hasError)
			return bodyElementResult.error;

		BodyStatment_Result<SuperStatement>& bodyElement = bodyElementResult.value();
		if (bodyElement.expression->getId() == SyntaxNodeId::EmptyStatment)
			return body;

		for (auto& statment_ : bodyElement.beforeStatment)
			body->addStatment(statment_);

		body->addStatment(bodyElement.expression);

		for (auto& statment_ : bodyElement.afterStatment)
			body->addStatment(statment_);
	}

	return ERROR_convertBody_outOfBounds(funcInfo, iterator);
}
