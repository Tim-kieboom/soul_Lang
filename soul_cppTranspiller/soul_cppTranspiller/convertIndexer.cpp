#include "convertIndexer.h"
#include "soulCheckers.h"

using namespace std;

Result<string> convertIndexer(TokenIterator& iterator, FuncInfo& funcInfo)
{
	stringstream ss;
	ss << '[';

	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ErrorInfo("unexpeced end while checking indexer", iterator.currentLine);

	if (getDuckType_fromValue(token) != DuckType::number)
		return ErrorInfo("invalid token in indexer token: \'" + token + "\'", iterator.currentLine);

	ss << token;

	if (!iterator.nextToken())
		return ErrorInfo("unexpeced end while checking indexer", iterator.currentLine);

	if (token != "]")
		return ErrorInfo("indexer doesn't end with \']\', token: \'" + token + "\'", iterator.currentLine);

	ss << ']';
	return ss.str();
}