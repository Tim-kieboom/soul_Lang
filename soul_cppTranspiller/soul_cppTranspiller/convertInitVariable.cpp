#include "convertInitVariable.h"
#include "soulChecker.h"
#include "EmptyStatment.h"
#include "convertAssignment.h"
using namespace std;

static const shared_ptr<SuperStatement> emptyStatment = make_shared<EmptyStatment>(EmptyStatment());

static inline ErrorInfo ERROR_convertInitVariable_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("enexpaced end while converting InitVariable", iterator.currentLine);
}

static inline Result<BodyStatment_Result<InitializeVariable>> _convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context, bool isInGlobalScope)
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
		if (type.isMutable)
			return ErrorInfo("\'" + varName + "\' must be inmutable because function is 'Functional'", iterator.currentLine);
	}

	Result<VarInfo*> varExsists = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
	{
		if(!varExsists.value()->isForwardDecl)
			return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);
	}

	VarInfo var = VarInfo(token, toString(type));

	if (isInGlobalScope)
		metaData.addToGlobalScope(var);
	else if(varExsists.hasError)
		context.scope.getCurrentNesting().addVariable(var);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	auto bodyResult = BodyStatment_Result<InitializeVariable>();

	if (token == ";")
	{
		if (!varExsists.hasError)
			varExsists.value()->isForwardDecl = false;

		bodyResult.expression = make_shared<InitializeVariable>(InitializeVariable(toString(type), varName, emptyStatment));
		return bodyResult;
	}

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	if (!iterator.nextToken(/*steps:*/-1))
		return ERROR_convertInitVariable_outOfBounds(iterator);

	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
	{
		if (!varExsists.value()->isForwardDecl)
			return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);

		varExsists.value()->isForwardDecl = false;
	}

	Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
	if (assignResult.hasError)
		return assignResult.error;

	bodyResult.addToBodyResult(assignResult.value());
	bodyResult.expression = make_shared<InitializeVariable>(InitializeVariable(toString(type), varName, assignResult.value().expression));
	return bodyResult;
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, RawType& type)
{
	vector<Nesting> nestings;
	nestings.emplace_back();
	CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(nestings)));
	Result<BodyStatment_Result<InitializeVariable>> bodyResult = _convertInitVariable(iterator, metaData, type, context, true);
	if (bodyResult.hasError)
		return bodyResult.error;

	return bodyResult.value();
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context)
{
	return _convertInitVariable(iterator, metaData, type, context, false);
}
