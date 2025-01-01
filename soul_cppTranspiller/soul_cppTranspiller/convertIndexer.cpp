#include "convertIndexer.h"
#include "soulCheckers.h"

using namespace std;

static inline DuckType _getDuckTypeOfToken(const string& token, const TokenIterator& iterator, ScopeIterator& scope, MetaData& metaData)
{
	Result<VarInfo> varResult = scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);
	if (varResult.hasError)
		return getDuckType_fromValue(token);

	VarInfo var = varResult.value();

	if (var.type.isComplexType)
		return DuckType::invalid;

	return getDuckType(var.type.primType);
}

Result<string> convertIndexer(TokenIterator& iterator, FuncInfo& funcInfo, ScopeIterator& scope, MetaData& metaData)
{
	stringstream ss;
	ss << '[';

	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ErrorInfo("unexpeced end while checking indexer", iterator.currentLine);

	if (_getDuckTypeOfToken(token, iterator, scope, metaData) != DuckType::number)
		return ErrorInfo("invalid token in indexer token: \'" + token + "\'", iterator.currentLine);

	ss << token;

	if (!iterator.nextToken())
		return ErrorInfo("unexpeced end while checking indexer", iterator.currentLine);

	if (token != "]")
		return ErrorInfo("indexer doesn't end with \']\', token: \'" + token + "\'", iterator.currentLine);

	ss << ']';
	return ss.str();
}