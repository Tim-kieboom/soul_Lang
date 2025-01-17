#include "convertInitVariable.h"
#include "soulChecker.h"
#include "convertAssignment.h"
using namespace std;

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

	Result<VarInfo*> varExsists = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
		return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);

	VarInfo var = VarInfo(token, toString(type));
	
	if (isInGlobalScope)
		metaData.addToGlobalScope(var);
	else
		context.scope.getCurrentNesting().addVariable(var);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	BodyStatment_Result<InitializeVariable> bodyResult
	(
		make_shared<InitializeVariable>(InitializeVariable(toString(type), varName))
	);

	if (token == ";")
		return bodyResult;

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	if (!iterator.nextToken(/*steps:*/-1))
		return ERROR_convertInitVariable_outOfBounds(iterator);

	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
		return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);

	Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
	if (assignResult.hasError)
		return assignResult.error;

	bodyResult.addToBodyResult(assignResult.value());
	bodyResult.afterStatment.push_back(assignResult.value().expression);
	return bodyResult;
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable_inGlobal(TokenIterator& iterator, MetaData& metaData, RawType& type)
{
	vector<Nesting> nestings;
	nestings.emplace_back();
	CurrentContext context = CurrentContext(ScopeIterator(nestings));
	Result<BodyStatment_Result<InitializeVariable>> bodyResult = _convertInitVariable(iterator, metaData, type, context, true);
	if (bodyResult.hasError)
		return bodyResult.error;

	return bodyResult.value();
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context)
{
	return _convertInitVariable(iterator, metaData, type, context, false);
}
