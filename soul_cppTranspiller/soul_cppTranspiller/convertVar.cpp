#include "convertVar.h"
#define ERROR_var ErrorInfo("incomplete functionBody funcName: \'" + string(funcInfo.funcName) + '\'', iterator.currentLine)

using namespace std;

static inline string toString(varSetter_Option& option)
{
	switch(option)
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

	string token;
	string symbool;

	if (!iterator.nextToken(/*out*/symbool))
		return ERROR_var;

	if (!initListEquals({ "+=", "-=", "*=", "/=", "=", "++", "--" }, symbool))
		return ErrorInfo("invalid symbol after variable symbool: \'" + symbool + "\'", iterator.currentLine);

	if (!iterator.nextToken(/*out*/token))
		return ERROR_var;

	if (initListEquals({ "++", "--" }, symbool))
	{
		if (token != toString(option))
			return ErrorInfo("variable incomplete atfer \'" + symbool + "\' need ';' but has: " + token, iterator.currentLine);

		ss << varInfo.name << symbool << toString(option) << "\n";
		return ss.str();
	}

	Result<string> varSetResult = convertVarSetter(iterator, metaData, varInfo.type, funcInfo, scope, option);
	if (varSetResult.hasError)
		return varSetResult.error;

	ss << varInfo.name << ' ' << symbool << ' ' << varSetResult.value();
	return ss.str();
}