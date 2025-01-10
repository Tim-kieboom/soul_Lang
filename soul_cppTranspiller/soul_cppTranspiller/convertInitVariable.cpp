#include "convertInitVariable.h"
#include "soulChecker.h"
#include "NoExpression.h"
#include "convertAssignment.h"
using namespace std;

static inline ErrorInfo ERROR_convertInitVariable_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("enexpaced end while converting InitVariable", iterator.currentLine);
}

Result<vector<shared_ptr<SuperStatement>>> convertInitVariable(TokenIterator& iterator, MetaData& metaData, RawType& type, CurrentContext& context)
{
	string& token = iterator.currentToken;
	vector<shared_ptr<SuperStatement>> statments;
	statments.reserve(2);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	if (!checkName(token))
		return ErrorInfo("name invalid name: \'" + token + "\'", iterator.currentLine);
	string varName = token;

	Result<VarInfo> varExsists = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (!varExsists.hasError)
		return ErrorInfo("varName: \'" + token + "\' already exsists", iterator.currentLine);

	VarInfo var = VarInfo(token, toString(type));
	context.scope.getCurrentNesting().addVariable(var);

	if (!iterator.nextToken())
		return ERROR_convertInitVariable_outOfBounds(iterator);

	statments.push_back
	(
		make_shared<InitializeVariable>(InitializeVariable(toString(type), varName))
	);

	if (token == ";")
		return statments;

	if (token != "=")
		return ErrorInfo("invalid symbool in argument, symbool: \'" + token + '\'', iterator.currentLine);

	if (!iterator.nextToken(/*steps:*/-1))
		return ERROR_convertInitVariable_outOfBounds(iterator);

	Result<shared_ptr<Assignment>> assignResult = convertAssignment(iterator, metaData, var, context);
	if (assignResult.hasError)
		return assignResult.error;

	statments.push_back(assignResult.value());
	return statments;
}
