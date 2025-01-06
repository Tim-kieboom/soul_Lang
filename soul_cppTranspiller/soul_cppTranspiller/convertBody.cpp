#include "convertBody.h"
#include "Assignment.h"
#include "convertAssignment.h"
using namespace std;

static inline ErrorInfo ERROR_convertBody_outOfBounds(FuncDeclaration& funcInfo, TokenIterator& iterator)
{
	return ErrorInfo("unexpected en in functionBody, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);
}

Result<FuncNode> convertBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context)
{
	unique_ptr<BodyNode> body = make_unique<BodyNode>();

	string checkBracket;
	if (!iterator.peekToken(checkBracket))
		return ERROR_convertBody_outOfBounds(funcInfo, iterator);

	if (checkBracket != "{")
		return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcInfo.functionName) + '\'', iterator.currentLine);

	string& token = iterator.currentToken;

	Result<string> result;
	uint32_t openCurlyBracketCounter = 0;
	while (iterator.nextToken())
	{
		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}
		else if (token == "}")
		{
			openCurlyBracketCounter--;

			if (openCurlyBracketCounter != 0)
				continue;

			return FuncNode(funcInfo, move(body));
		}

		Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
		Result<VarInfo> varResult = context.scope.tryGetVariable_fromCurrent(token, metaData.globalScope, iterator.currentLine);

		if(!varResult.hasError)
		{
			Result<Assignment> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
			if (assignResult.hasError)
				return assignResult.error;

		}
	}

	return ERROR_convertBody_outOfBounds(funcInfo, iterator);
}
