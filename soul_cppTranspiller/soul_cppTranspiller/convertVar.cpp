#include "convertVar.h"
#include "soulCheckers.h"
#include "convertIndexer.h"

using namespace std;

static inline ErrorInfo ERROR_convertVar_outOfBounds(FuncInfo& funcInfo, TokenIterator& iterator)
{
	return ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine);
}

static inline string toString(varSetter_Option& option)
{
	switch (option)
	{
	case varSetter_Option::endComma:
		return ",";

	case varSetter_Option::endSemiColon:
		return ";";

	case varSetter_Option::endRoundBracket:
		return ")";

	default:
		throw std::exception("invalid varSetter_Option");
	}
}

static inline Result<void> _isSymboolAllowed(string& symbool, const VarInfo& varInfo, const TokenIterator& iterator)
{
	static const initializer_list<const char*> allowed_ArraySymbols = { "[", "=" };
	static const initializer_list<const char*> allowed_PointerSymbols = { ".", "=" };
	static const initializer_list<const char*> allowed_DefaultSymbols = { "+=", "-=", "*=", "/=", "=", "++", "--", "." };
	static const initializer_list<const char*> allowed_ConstSymbols = { ".", "[" };

	initializer_list<const char*> allowedSymbols;
	if (varInfo.type.isArray)
	{
		allowedSymbols = allowed_ArraySymbols;
	}
	else if (varInfo.type.isPointer)
	{
		allowedSymbols = allowed_PointerSymbols;
	}
	else
	{
		allowedSymbols = allowed_DefaultSymbols;
	}

	if (!varInfo.type.isMutable && !initListEquals(allowed_ConstSymbols, symbool))
		return ErrorInfo("can not change a const value, var: \'" + varInfo.name + "\', type: \'" + toString(varInfo.type) + "\'", iterator.currentLine);

	if (!initListEquals(allowedSymbols, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (varInfo.type.isComplexType && symbool != "=")
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	return {};
}

Result<string> convertVar(VarInfo& varInfo, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope, varSetter_Option option)
{
	stringstream ss;
	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ERROR_convertVar_outOfBounds(funcInfo, iterator);

	string symbool = token;
	Result<void> isAllowed = _isSymboolAllowed(symbool, varInfo, iterator);
	if (isAllowed.hasError)
		return isAllowed.error;

	if (symbool == "[")
	{
		Result<string> indexResult = convertIndexer(/*out*/iterator, /*out*/funcInfo);
		if (indexResult.hasError)
			return indexResult.error;

		symbool = indexResult.value();
	}

	if (!iterator.nextToken())
		return ERROR_convertVar_outOfBounds(funcInfo, iterator);

	if (initListEquals({ "++", "--" }, symbool))
	{
		if(!varInfo.type.isComplexType && getDuckType(varInfo.type.primType) != DuckType::number)
			return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

		if (token != toString(option))
			return ErrorInfo("variable incomplete atfer \'" + symbool + "\' need ';' but has: " + token, iterator.currentLine);

		ss << varInfo.name << symbool << toString(option);
		if(metaData.transpillerOption.addEndLines)
			ss << '\n';
		return ss.str();
	}

	Result<string> varSetResult = convertVarSetter(iterator, metaData, varInfo.type, funcInfo, scope, option);
	if (varSetResult.hasError)
		return varSetResult.error;

	ss << varInfo.name << ' ' << symbool << ' ' << varSetResult.value();
	return ss.str();
}
