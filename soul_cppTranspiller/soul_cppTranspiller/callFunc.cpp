#include "callFunc.h"
#include <sstream>
#include <algorithm>

using namespace std;

static inline Result<pair<string, uint32_t>> getCallArgument(TokenIterator& iterator, MetaData& metaData, const FuncInfo& inCurrentFunc, const FuncInfo& callFunc, bool& stop)
{
	stringstream ss;
	string token;

	uint32_t openBracketStack;

	while(iterator.nextToken(/*out*/token))
	{
		if(token == "(")
		{
			openBracketStack++;
			ss << '(';
			continue;
		}
		else if(token == ")")
		{
			openBracketStack--;
			ss << ')';
			if (openBracketStack == 0)
				return ErrorInfo("function caller ended unexpectedlly", iterator.currentLine);

			continue;
		}


	}

	return ErrorInfo("function caller incomplete", iterator.currentLine);
}


Result<string> convertFuncCall(TokenIterator& iterator, MetaData& metaData, FuncInfo callFunc, FuncInfo funcInfo)
{
	stringstream ss;
	string token;

	if (!iterator.nextToken(/*out*/token, /*step:*/0))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (token != callFunc.funcName)
		return ErrorInfo("function call, functionName invalid", iterator.currentLine);
	ss << token;

	if (!iterator.nextToken(/*out*/token))
		return ErrorInfo("incomplete functionBody funcName: " + string(funcInfo.funcName), iterator.currentLine);;

	if (token != "(")
		return ErrorInfo("function call has to start with \'(\'", iterator.currentLine);

	ss << '(';

	uint32_t openBracketStack = 1;
	uint32_t argCounter = 0;
	vector<pair<string, uint32_t>> argsStrings;
	argsStrings.reserve(callFunc.args.size());

	sort
	(
		argsStrings.begin(), argsStrings.end(),
		[](const pair<string, uint32_t>& a, const pair<string, uint32_t>& b)
		{
			return a.second < b.second;
		}
	);

	for (const auto& pair : argsStrings)
		ss << pair.first;
	ss << ')';
	return ss.str();
}
