#include "convertInitVariable.h"
#include "soulChecker.h"
#include "EmptyStatment.h"
#include "convertAssignment.h"
#include "convertExpression.h"
#include "Variable.h"
using namespace std;

static const shared_ptr<SuperStatement> emptyStatment = make_shared<EmptyStatment>(EmptyStatment());

static inline ErrorInfo ERROR_convertInitVariable_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("enexpaced end while converting InitVariable", iterator.currentLine);
}

static inline bool isVarVariable(RawType* type)
{
	return type == nullptr;
}

static inline Result<BodyStatment_Result<InitializeVariable>> _convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType* type, CurrentContext& context, bool isInGlobalScope, bool isVariableConstant)
{
	string& token = iterator.currentToken;
	vector<shared_ptr<SuperStatement>> statments;
	statments.reserve(2);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	if (!checkName(token))
		return ErrorInfo("name invalid name: \'" + token + "\'", iterator.currentLine);
	string varName = token;


	if (context.functionRuleSet == CurrentContext::FuncRuleSet::Functional)
	{
		if (!isVariableConstant)
			return ErrorInfo("\'" + varName + "\' must be inmutable because function is 'Functional'", iterator.currentLine);
	}

	Result<VarInfo*> varExsists = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
	{
		if(!varExsists.value()->isForwardDecl)
			return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);
	}

	string rawType = (isVarVariable(type)) ? "<incomplete-var>" : toString(*type);
	VarInfo var = VarInfo(token, rawType);

	if (isInGlobalScope)
		metaData.addToGlobalScope(var);
	else if(varExsists.hasError)
		context.scope.getCurrentNesting().addVariable(var);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	auto bodyResult = BodyStatment_Result<InitializeVariable>();

	if (token == ";")
	{
		if (isVarVariable(type))
			return ErrorInfo("can not use 'var' or 'const' without assignment", iterator.currentLine);

		if (!varExsists.hasError)
			varExsists.value()->isForwardDecl = false;

		bodyResult.expression = make_shared<InitializeVariable>(InitializeVariable(toString(*type), varName, emptyStatment));
		return bodyResult;
	}

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
	{
		if (!varExsists.value()->isForwardDecl)
			return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);

		varExsists.value()->isForwardDecl = false;
	}

	if (isVarVariable(type))
	{
		if (!iterator.nextToken())
			return ERROR_convertInitVariable_outOfBounds(iterator);

		RawType assignType;
		Result<BodyStatment_Result<SuperExpression>> assignExpression = convertExpression(iterator, metaData, context, { ";" }, !isVariableConstant, &assignType);
		if (assignExpression.hasError)
			return assignExpression.error;

		assignType.isMutable = !isVariableConstant;
		varResult.value()->stringedRawType = toString(assignType);

		auto setVariable = make_shared<Variable>(Variable(varName));
		shared_ptr<Assignment> assignment = make_shared<Assignment>
			(
				Assignment(setVariable, assignExpression.value().expression)
			);

		bodyResult.addToBodyResult(assignExpression.value());
		bodyResult.expression = make_shared<InitializeVariable>(InitializeVariable(varResult.value()->stringedRawType, varName, assignment));
		return bodyResult;
	}

	if (!iterator.nextToken(/*steps:*/-1))
		return ERROR_convertInitVariable_outOfBounds(iterator);

	Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
	if (assignResult.hasError)
		return assignResult.error;

	bodyResult.addToBodyResult(assignResult.value());
	bodyResult.expression = make_shared<InitializeVariable>(InitializeVariable(varResult.value()->stringedRawType, varName, assignResult.value().expression));
	return bodyResult;
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, RawType& type)
{
	vector<Nesting> nestings;
	nestings.emplace_back();
	CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(nestings)));
	Result<BodyStatment_Result<InitializeVariable>> bodyResult = _convertInitVariable(iterator, metaData, &type, context, true, !type.isMutable);
	if (bodyResult.hasError)
		return bodyResult.error;

	return bodyResult.value();
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, bool isConst)
{
	vector<Nesting> nestings;
	nestings.emplace_back();
	CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(nestings)));
	Result<BodyStatment_Result<InitializeVariable>> bodyResult = _convertInitVariable(iterator, metaData, nullptr, context, true, isConst);
	if (bodyResult.hasError)
		return bodyResult.error;

	return bodyResult.value();
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context)
{
	return _convertInitVariable(iterator, metaData, &type, context, false, !type.isMutable);
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, bool isConst)
{
	return _convertInitVariable(iterator, metaData, nullptr, context, false, isConst);
}
