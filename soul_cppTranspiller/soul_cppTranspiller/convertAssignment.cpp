#include "convertAssignment.h"
#include "stringTools.h"

using namespace std;

static inline ErrorInfo ERROR_convertVar_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("unexpected end while converting Assingment", iterator.currentLine);
}

static inline Result<void> _isSymboolAllowed(string& symbool, MetaData& metaData, VarInfo& varInfo, RawType& type, const TokenIterator& iterator)
{
	static const initializer_list<const char*> allowed_ArraySymbols = { "[", "=" };
	static const initializer_list<const char*> allowed_PointerSymbols = { ".", "=" };
	static const initializer_list<const char*> allowed_DefaultSymbols = { "+=", "-=", "*=", "/=", "=", "++", "--", "." };
	static const initializer_list<const char*> allowed_ConstSymbols = { ".", "[" };

	initializer_list<const char*> allowedSymbols;

	if (type.isArray())
	{
		allowedSymbols = allowed_ArraySymbols;
	}
	else if (type.isPointer())
	{
		allowedSymbols = allowed_PointerSymbols;
	}
	else
	{
		allowedSymbols = allowed_DefaultSymbols;
	}

	if (!type.isMutable && !initListEquals(allowed_ConstSymbols, symbool))
		return ErrorInfo("can not change a const value, var: \'" + varInfo.name + "\', type: \'" + toString(varInfo.type) + "\'", iterator.currentLine);

	if (!initListEquals(allowedSymbols, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (metaData.isClass(type) && symbool != "=")
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	return {};
}

Result<Assignment> convertAssignment(TokenIterator& iterator, MetaData& metaData, VarInfo& varInfo, CurrentContext& context)
{
	Result<RawType> typeResult = getRawType_fromStringedRawType(varInfo.stringedRawType, metaData.classStore, iterator.currentLine);
	if (typeResult.hasError)
		return typeResult.error;

	RawType type = typeResult.value();

	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ERROR_convertVar_outOfBounds(iterator);

	string symbool = token;
	Result<void> isAllowed = _isSymboolAllowed(symbool, metaData, varInfo, type, iterator);
	if (isAllowed.hasError)
		return isAllowed.error;

	if (symbool == "[")
	{
		//indexer
		throw exception("not yet implemented");
	}

	if (!iterator.nextToken())
		return ERROR_convertVar_outOfBounds(iterator);

	if (initListEquals({ "++", "--" }, symbool))
	{
		Result<PrimitiveType, ClassInfo> trueType;
		if (!type.getType(metaData.classStore, trueType))
			return ErrorInfo("variable type is invalid", iterator.currentLine);

		bool isClass = trueType.hasError;

		if(!isClass && getDuckType(trueType.value()) != DuckType::number)
			return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);


	}

}
