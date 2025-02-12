#include "SuperExpression_ToCpp.h"
#include <sstream>

#include "Literal.h"
#include "Variable.h"
#include "Refrence.h"
#include "Increment.h"
#include "IndexArray.h"
#include "soulChecker.h"
#include "FunctionCall.h"
#include "StringLiteral.h"
#include "ConstructArray.h"
#include "CopyExpression.h"
#include "soulToCpp.Type.h"
#include "RangeExpression.h"
#include "BinairyExpression.h"
using namespace std;

static inline const char* cppOperator(SyntaxTree_Operator opType, bool& isValid)
{
	isValid = true;

	switch (opType)
	{
	case SyntaxTree_Operator::Equals:
		return "==";
	case SyntaxTree_Operator::NotEquals:
		return "!=";
	case SyntaxTree_Operator::IsSmaller:
		return "<";
	case SyntaxTree_Operator::IsSmaller_Equals:
		return "<=";
	case SyntaxTree_Operator::IsBigger:
		return ">";
	case SyntaxTree_Operator::IsBigger_Equals:
		return ">=";

	case SyntaxTree_Operator::Add:
		return "+";
	case SyntaxTree_Operator::Sub:
		return "-";
	case SyntaxTree_Operator::Mul:
		return "*";
	case SyntaxTree_Operator::Div:
		return "/";
	case SyntaxTree_Operator::Modulo:
		return "%";


	case SyntaxTree_Operator::BitWise_OR:
		return "|";
	case SyntaxTree_Operator::BitWise_AND:
		return "&";
	case SyntaxTree_Operator::BitWise_XOR:
		return "^";
	case SyntaxTree_Operator::Logical_OR:
		return "||";
	case SyntaxTree_Operator::Logical_AND:
		return "&&";

	case SyntaxTree_Operator::Invalid:
	case SyntaxTree_Operator::Pow:
	case SyntaxTree_Operator::Root:
	default:
		isValid = false;
		return "";
	}
}

static inline Result<string> _convertBinairyExpression(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	constexpr const char* pow_FuncName = "std::pow";
	constexpr const char* CompileConstPow_FuncName = "__Soul_CompileConst_math__::pow";

	constexpr const char* root_FuncName = "root";
	constexpr const char* CompileConstRoot_FuncName = "__Soul_CompileConst_math__::root";

	constexpr const char* log_FuncName = "log";
	constexpr const char* CompileConstLog_FuncName = "__Soul_CompileConst_math__::log";

	stringstream ss;
	shared_ptr<BinaryExpression> binairy = dynamic_pointer_cast<BinaryExpression>(expression);
	SyntaxTree_Operator& opType = binairy->operatorType;
	
	Result<string> left = SuperExpression_ToCpp(binairy->left, metaData, context);
	if (left.hasError)
		return left.error;

	Result<string> right = SuperExpression_ToCpp(binairy->right, metaData, context);
	if (right.hasError)
		return right.error;

	bool isCompileConst = context.functionRuleSet == CurrentContext::FuncRuleSet::Functional || isExpression_CompileConstant(expression, metaData, context);

	if(opType == SyntaxTree_Operator::Pow)
	{
		const char* pow = (isCompileConst) ? CompileConstPow_FuncName : pow_FuncName;
		ss << pow << "(" << left.value() << ", " << right.value() << ')';
	}
	else if(opType == SyntaxTree_Operator::Root)
	{
		const char* root = (isCompileConst) ? CompileConstRoot_FuncName : root_FuncName;
		ss << root << "(" << right.value() << ", "<< left.value() << ')';
	}
	else if(opType == SyntaxTree_Operator::Log)
	{
		const char* log = (isCompileConst) ? CompileConstLog_FuncName : log_FuncName;
		ss << log << "(" << right.value() << ", " << left.value() << ')';
	}
	else
	{
		bool isValid;
		ss << left.value() << cppOperator(opType, /*out*/isValid) << right.value();
		if (!isValid)
			return ErrorInfo("cppOperator invalid in BinairyExpression: \'" + binairy->printToString() + "\'", 0);
	}

	return ss.str();
}

static inline Result<string> _convertCopyExpression(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	constexpr const char* copyFuncName = "__Soul_copy__";

	stringstream ss;
	shared_ptr<CopyExpression> copy = dynamic_pointer_cast<CopyExpression>(expression);
	
	Result<string> arg = SuperExpression_ToCpp(copy->copyExpression, metaData, context);
	if (arg.hasError)
		return arg.error;

	ss << copyFuncName << '(' << arg.value();

	if(copy->argument->getId() != SyntaxNodeId::EmptyExpresion)
	{
		Result<string> copyArg = SuperExpression_ToCpp(copy->argument, metaData, context);
		if (copyArg.hasError)
			return copyArg.error;

		ss << ", " << copyArg.value();
	}

	ss << ')';

	return ss.str();
}

static inline Result<string> _convertConstructArray(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;
	shared_ptr<ConstructArray> ctorArray = dynamic_pointer_cast<ConstructArray>(expression);
	Result<RawType> typeResult = getRawType_fromStringedRawType(ctorArray->type, metaData.classStore, context.currentTemplateTypes, 0);
	if (typeResult.hasError)
		return typeResult.error;

	Result<void> result = typeResult.value().addTypeWrapper(TypeWrapper::array_, 0);
	if (result.hasError)
		return result.error;

	Result<string> cppType = soulToCpp_Type(typeResult.value(), metaData, context);
	if (cppType.hasError)
		return cppType.error;

	Result<string> amount = SuperExpression_ToCpp(ctorArray->amountElements, metaData, context);
	if (amount.hasError)
		return amount.error;

	ss << cppType.value() << '(' << amount.value() << ')';
	return ss.str();
}

static inline Result<string> _convertFunctionCall(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;
	shared_ptr<FunctionCall> funcCall = dynamic_pointer_cast<FunctionCall>(expression);
	vector<shared_ptr<SuperExpression>> args = funcCall->args;

	ss << funcCall->funcName << '(';
	for (uint32_t i = 0; i < args.size(); i++)
	{
		auto& arg = args[i];
		Result<string> argResult = SuperExpression_ToCpp(arg, metaData, context);
		if (argResult.hasError)
			return argResult.error;

		ss << argResult.value();

		if (i < args.size() - 1)
			ss << ", ";
	}
	ss << ')';

	return ss.str();
}

static inline Result<string> _convertIndexArray(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	constexpr const char* rangeMethode = "__soul_makeSpan_fail__";
	constexpr const char* rangeClass = "__Soul_Range__";

	stringstream ss;
	shared_ptr<IndexArray> indexArray = dynamic_pointer_cast<IndexArray>(expression);
	string& arrayName = indexArray->arrayName;
	shared_ptr<SuperExpression>& index = indexArray->index;

	ss << arrayName;

	if(index->getId() == SyntaxNodeId::RangeExpression)
	{
		shared_ptr<SuperExpression>& end = dynamic_pointer_cast<RangeExpression>(index)->end;
		shared_ptr<SuperExpression>& begin = dynamic_pointer_cast<RangeExpression>(index)->begin;

		Result<string> endResult = SuperExpression_ToCpp(end, metaData, context);
		if (endResult.hasError)
			return endResult.error;

		Result<string> beginResult = SuperExpression_ToCpp(begin, metaData, context);
		if (beginResult.hasError)
			return beginResult.error;


		if (end->getId() == SyntaxNodeId::EmptyExpresion && begin->getId() == SyntaxNodeId::EmptyExpresion)
			return ErrorInfo("RangeExpression can not have an empty begin and empty end at the same time: \'" + index->printToString() + "\'", 0);

		if(end->getId() == SyntaxNodeId::EmptyExpresion)
		{
			ss << '.' << rangeMethode << '(' << rangeClass << '(' << beginResult.value() << ", __Soul_Range__::RangeType::START))";
		}
		else if(begin->getId() == SyntaxNodeId::EmptyExpresion)
		{
			ss << '.' << rangeMethode << '(' << rangeClass << '(' << endResult.value() << ", __Soul_Range__::RangeType::END))";
		}
		else
		{
			ss << '.' << rangeMethode << '(' << rangeClass << '(' << beginResult.value() << ", " << endResult.value() << "))";
		}
	}
	else
	{
		Result<string> indexResult = SuperExpression_ToCpp(index, metaData, context);
		if (indexResult.hasError)
			return indexResult.error;

		ss << '[' << indexResult.value() << ']';
	}

	return ss.str();
}

static inline Result<string> _convertIncrement(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
	stringstream ss;
	shared_ptr<Increment> increment = dynamic_pointer_cast<Increment>(expression);

	if (increment->incrementAmount > 1)
		throw exception("increment more then 1 is not implemented");

	const char* incrementOperator = (increment->isNegative) ? "--" : "++";
	Result<string> varResult = SuperExpression_ToCpp(increment->variable, metaData, context);
	if (varResult.hasError)
		return varResult.error;

	string& var = varResult.value();
	
	if (increment->isBeforeVar)
		ss << incrementOperator << var;
	else
		ss << var << incrementOperator;

	return ss.str();
}

Result<string> SuperExpression_ToCpp(shared_ptr<SuperExpression> expression, MetaData& metaData, CurrentContext& context)
{
    switch(expression->getId())
    {
	case SyntaxNodeId::Literal:
		return dynamic_pointer_cast<Literal>(expression)->value;

	case SyntaxNodeId::Variable:
		return dynamic_pointer_cast<Variable>(expression)->varName;

	case SyntaxNodeId::Refrence:
		return SuperExpression_ToCpp(dynamic_pointer_cast<Refrence>(expression)->expression, metaData, context);

	case SyntaxNodeId::Increment:
		return _convertIncrement(expression, metaData, context);

	case SyntaxNodeId::IndexArray:
		return _convertIndexArray(expression, metaData, context);

	case SyntaxNodeId::FunctionCall:
		return _convertFunctionCall(expression, metaData, context);

	case SyntaxNodeId::StringLiteral:
		return dynamic_pointer_cast<StringLiteral>(expression)->value;

	case SyntaxNodeId::EmptyExpresion:
		return string();

	case SyntaxNodeId::ConstructArray:
		return _convertConstructArray(expression, metaData, context);

	case SyntaxNodeId::CopyExpression:
		return _convertCopyExpression(expression, metaData, context);

	case SyntaxNodeId::RangeExpression:
		throw exception("rangeExpression not implemeted here (impl in IndexArray)");

	case SyntaxNodeId::BinairyExpression:
		return _convertBinairyExpression(expression, metaData, context);

	default:
		return ErrorInfo("\'" + expression->printToString() + "\' is not a valid cpp SuperExpression", 0);
    }
}
