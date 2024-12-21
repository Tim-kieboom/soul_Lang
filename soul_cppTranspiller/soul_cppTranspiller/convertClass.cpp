#include "convertClass.h"
#include "soulCheckers.h"

using namespace std;

static inline ErrorInfo ERROR_convertClass_OutOfBounds(TokenIterator& iterator, string className)
{
	return ErrorInfo("enexpected end of class: \'" + className + "\'", iterator.currentLine);
}

static inline Result<string> convertClassBody(TokenIterator& iterator, MetaData& metaData, const string& className)
{
	stringstream ss;
	string& token = iterator.currentToken;

	string checkBracket;
	if (!iterator.peekToken(checkBracket))
		return ERROR_convertClass_OutOfBounds(iterator, className);

	if (checkBracket != "{")
		return ErrorInfo("no '{' after class declaration, className: \'" + className + '\'', iterator.currentLine);

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';

	ss << '{';
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> result;
	uint32_t openCurlyBracketCounter = 0;
	while (iterator.nextToken())
	{
		Result<TypeInfo> typeResult = getTypeInfo(iterator, metaData.classStore);

		if(!typeResult.hasError)
		{

		}
		else
		{
			return ErrorInfo("token invalid, token: \'" + token + "\'", iterator.currentLine);
		}
	}
	return ERROR_convertClass_OutOfBounds(iterator, className);
}

Result<string> convertClass(TokenIterator& iterator, MetaData& metaData)
{
	stringstream ss;
	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, "<name-unknown>");

	if (!checkName(token))
		return ErrorInfo("name of class is not allowed, name: \'" +token+ "\'", iterator.currentLine);
	string className = token;
	ss << "class " << className;

	Result<string> bodyResult = convertClassBody(iterator, metaData, className);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}
