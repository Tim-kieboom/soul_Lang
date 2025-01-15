#include "convertInitVariable.h"
#include "soulChecker.h"
#include "convertAssignment.h"
using namespace std;

static inline ErrorInfo ERROR_convertInitVariable_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("enexpaced end while converting InitVariable", iterator.currentLine);
}

Result<BodyStatment_Result<InitializeVariable>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context)
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
