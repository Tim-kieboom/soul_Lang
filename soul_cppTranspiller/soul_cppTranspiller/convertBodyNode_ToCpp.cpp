#include "convertBodyNode_ToCpp.h"
#include <sstream>

#include "BodyNode.h"
#include "IndexArray.h"
#include "ForStatment.h"
#include "WhileStatment.h"
#include "ConstructArray.h"
#include "ReturnStatment.h"
#include "RawArrayToSoulArray.h"
#include "FunctionCallStatment.h"
#include "SuperExpression_ToCpp.h"
#include "convertAssignment_ToCpp.h"
#include "If_else_elseIf_Statments.h"
#include "convertInitializeVariable_ToCpp.h"
#include "convertCompileConstVariable_ToCpp.h"

using namespace std;

static inline Result<string> _convertIfStatment(shared_ptr<SuperStatement>& node, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;

	shared_ptr<IfStatment> ifStatment = dynamic_pointer_cast<IfStatment>(node);
	Result<string> condition = SuperExpression_ToCpp(ifStatment->expression, metaData, context);
	if (condition.hasError)
		return condition.error;

	Result<string> body = convertBodyNode_ToCpp(ifStatment->body, metaData);
	if (body.hasError)
		return body.error;

	ss << "if("  << condition.value() << ")\n" << body.value();
	return ss.str();
}

static inline Result<string> _convertElseIfStatment(shared_ptr<SuperStatement>& node, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;

	shared_ptr<ElseIfStatment> elseIfStatment = dynamic_pointer_cast<ElseIfStatment>(node);
	Result<string> condition = SuperExpression_ToCpp(elseIfStatment->expression, metaData, context);
	if (condition.hasError)
		return condition.error;

	Result<string> body = convertBodyNode_ToCpp(elseIfStatment->body, metaData);
	if (body.hasError)
		return body.error;

	ss << "else if(" << condition.value() << ")\n" << body.value();
	return ss.str();
}

static inline Result<string> _convertElseStatment(shared_ptr<SuperStatement>& node, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;

	shared_ptr<ElseStatment> elseStatment = dynamic_pointer_cast<ElseStatment>(node);

	Result<string> body = convertBodyNode_ToCpp(elseStatment->body, metaData);
	if (body.hasError)
		return body.error;

	ss << "else\n" << body.value();
	return ss.str();
}

static inline Result<string> _convertForStatment(shared_ptr<SuperStatement>& node, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;

	shared_ptr<ForStatment> forStatment = dynamic_pointer_cast<ForStatment>(node);
	Nullable<shared_ptr<InitializeVariable>>& initCounter = forStatment->initCounter;

	ss << "for(";
	if (initCounter.isNull())
	{
		ss << ";";
	}
	else
	{
		Result<string> init = convertInitializeVariable_ToCpp(initCounter.value(), metaData, context);
		if (init.hasError)
			return init.error;

		ss << init.value();
	}

	Result<string> stopCondition = SuperExpression_ToCpp(forStatment->stopCondition, metaData, context);
	if (stopCondition.hasError)
		return stopCondition.error;

	ss << stopCondition.value() << ';';

	Result<string> doAfterEachLoop = SuperExpression_ToCpp(forStatment->doAfterEachLoop, metaData, context);
	if (doAfterEachLoop.hasError)
		return doAfterEachLoop.error;

	ss << doAfterEachLoop.value() << ')';

	Result<string> body = convertBodyNode_ToCpp(forStatment->body, metaData);
	if (body.hasError)
		return body.error;

	return ss.str();
}

static inline Result<string> _convertWhileStatment(shared_ptr<SuperStatement>& node, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;

	shared_ptr<WhileStatment> whileStatment = dynamic_pointer_cast<WhileStatment>(node);
	Result<string> condition = SuperExpression_ToCpp(whileStatment->condition, metaData, context);
	if (condition.hasError)
		return condition.error;

	Result<string> body = convertBodyNode_ToCpp(whileStatment->body, metaData);
	if (body.hasError)
		return body.error;

	ss << "while(" << condition.value() << ")\n" << body.value();
	return ss.str();
}

static inline Result<string> _convertReturnStatment(shared_ptr<SuperStatement>& statment, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;
	shared_ptr<ReturnStatment> returnStatment = dynamic_pointer_cast<ReturnStatment>(statment);

	Result<string> returnValue = SuperExpression_ToCpp(returnStatment->expression, metaData, context);
	if (returnValue.hasError)
		return returnValue.error;

	ss << "return " << returnValue.value() << ';';
	return ss.str();
}

static inline Result<string> _convertRawArrayToSoulArray(shared_ptr<SuperStatement>& statment, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;
	shared_ptr<RawArrayToSoulArray> convertArray = dynamic_pointer_cast<RawArrayToSoulArray>(statment);

	Result<RawType> type = getRawType_fromStringedRawType(convertArray->type, metaData.classStore, context.currentTemplateTypes, 0);
	if (type.hasError)
		return type.error;

	Result<RawType> arrayLessType = type.value().getTypeChild(0);
	if (arrayLessType.hasError)
		return arrayLessType.error;

	auto assignSoulArray = make_shared<Assignment>(convertArray->soulArray, make_shared<ConstructArray>(ConstructArray(toString(arrayLessType.value()), convertArray->arraySize)));
	auto init = make_shared<InitializeVariable>(InitializeVariable(convertArray->type, convertArray->soulArray->varName, assignSoulArray));

	Result<string> initSoul = convertInitializeVariable_ToCpp(init, metaData, context);
	if (initSoul.hasError)
		return initSoul.error;

	ss << initSoul.value() << "\nfor(uint64_t __CPP_i__ = 0; __CPP_i__ < " << convertArray->arraySize->varName << "; __CPP_i__++)\n\t" << convertArray->soulArray->varName << ".__soul_UNSAFE_at__(__CPP_i__)=" << convertArray->rawArray->varName << "[__CPP_i__];";
	return ss.str();
}

static inline Result<string> _convertStatment(shared_ptr<SuperStatement>& statment, MetaData& metaData, CurrentContext& context)
{
	switch (statment->getId())
	{
		case SyntaxNodeId::FieldNode:
			throw exception("not yet impl");
		case SyntaxNodeId::MethodeNode:
			throw exception("not yet impl");


		case SyntaxNodeId::Assignment:
			return convertAssignment_ToCpp(statment, metaData, context);

		case SyntaxNodeId::InitializeVariable:
			return convertInitializeVariable_ToCpp(statment, metaData, context);

		case SyntaxNodeId::CompileConstVariable:
			return convertCompileConstVariable_ToCpp(statment, metaData, context);

		case SyntaxNodeId::RawArrayToSoulArray:
			return _convertRawArrayToSoulArray(statment, metaData, context);

		case SyntaxNodeId::FunctionCallStatment:
		{
			Result<string> funcCall = SuperExpression_ToCpp(dynamic_pointer_cast<FunctionCallStatment>(statment)->functionCall, metaData, context);
			if (funcCall.hasError)
				return funcCall.error;

			return funcCall.value() + ";";
		}
		break;


		case SyntaxNodeId::IfStatment:
			return _convertIfStatment(statment, metaData, context);

		case SyntaxNodeId::ElseStatment:
			return _convertElseStatment(statment, metaData, context);

		case SyntaxNodeId::ElseIfStatment:
			return _convertElseIfStatment(statment, metaData, context);


		case SyntaxNodeId::ForStatment:
			return _convertForStatment(statment, metaData, context);

		case SyntaxNodeId::WhileStatment:
			return _convertWhileStatment(statment, metaData, context);



		case SyntaxNodeId::EmptyStatment:
			return string("");

		case SyntaxNodeId::BreakStatment:
			return string("break;");

		case SyntaxNodeId::ContinueStatment:
			return string("continue;");


		case SyntaxNodeId::ReturnStatment:
			return _convertReturnStatment(statment, metaData, context);


		default:
			return ErrorInfo("\'" + statment->printToString() + "\' is invalid as statment", 0);
	}
}

Result<string> convertBodyNode_ToCpp(shared_ptr<BodyNode> body, MetaData& metaData)
{
	stringstream ss;

	CurrentContext context = CurrentContext(ScopeIterator(body->context.scope.scope));
	ss << "{\n";
	for(shared_ptr<SuperStatement>& statment : body->statements)
	{
		Result<string> statmentResult = _convertStatment(statment, metaData, context);
		if (statmentResult.hasError)
			return statmentResult.error;

		ss << statmentResult.value() << '\n';
	}
	ss << "\n}\n";

	return ss.str();
}
