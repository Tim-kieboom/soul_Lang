#include "convertVar.h"

using namespace std;

static inline ErrorInfo ERROR_convertVer_outOfBounds(FuncInfo& funcInfo, TokenIterator& iterator)
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

Result<string> convertVar(VarInfo& varInfo, TokenIterator& iterator, MetaData& metaData, FuncInfo& funcInfo, ScopeIterator& scope, varSetter_Option option)
{
	stringstream ss;

	string& token = iterator.currentToken;

	if (!iterator.nextToken())
		return ERROR_convertVer_outOfBounds(funcInfo, iterator);

	string symbool = token;
	if (!initListEquals({ "+=", "-=", "*=", "/=", "=", "++", "--" }, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if(varInfo.type.isComplexType && symbool != "=")
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (!iterator.nextToken())
		return ERROR_convertVer_outOfBounds(funcInfo, iterator);

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
