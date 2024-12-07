#include "transpiller.h"
#include <sstream>

#include "ScopeIterator.h"
#include "internalFunction.h"
#include "convertFuncDecleration.h"
#include "convertBody.h"

using namespace std;

static void addC_strToGlobalScope(MetaData& metaData)
{
	for (const auto& pair : metaData.c_strStore)
	{
		VarInfo c_str = VarInfo
		(
			pair.second.name, 
			TypeInfo(PrimitiveType::str, /*isMutable:*/false), 
			/*isOpional:*/false
		);
		metaData.addToGlobalScope(c_str);
	}
}

static void addInternalFunctions_ToMetaData(/*out*/ MetaData& metaData)
{
	for (const FuncInfo& funcInfo : internalFunctions)
		metaData.addFuncInfo(string(funcInfo.funcName), funcInfo);
}

static void addConstStrings_ToFile(/*out*/ stringstream& ss, const MetaData& metaData)
{
	const TranspilerOptions& option = metaData.transpillerOption;
	if (option.addEndLines)
		ss << "\n";

	for (const auto& keyValue : metaData.c_strStore)
	{
		const C_strPair& c_str = keyValue.second;
		ss << "constexpr const char* " << c_str.name << " = " << c_str.value << ";";
		if(option.addEndLines)
			ss << "\n";
	}
	if (option.addEndLines)
		ss << "\n";
}

Result<std::string> transpileToCpp(const std::vector<Token> tokens, MetaData& metaData)
{
	stringstream ss;

	metaData.addCppInclude("utility", "#include <utility>");
	metaData.addCppInclude("cstdint", "#include <cstdint>");
	metaData.addCppInclude("sstream", "#include <sstream>");
	metaData.addCppInclude("utility", "#include <utility>");
	metaData.addCppInclude("assert.h", "#include <assert.h>");
	metaData.addCppInclude("iostream", "#include <iostream>");
	metaData.addCppInclude("stdexcept", "#include <stdexcept>");

	addC_strToGlobalScope(/*out*/metaData);
	addInternalFunctions_ToMetaData(/*out*/metaData);
	addConstStrings_ToFile(/*out*/ss, metaData);

	TokenIterator iterator(tokens);
	string& token = iterator.currentToken;
	while(iterator.nextToken())
	{
		if(token == "func")
		{
			FuncInfo funcInfo;
			Result<string> funcDecl = convertFuncDeclaration(/*out*/iterator, /*out*/metaData, /*out*/funcInfo);
			if (funcDecl.hasError)
				return funcDecl.error;

			ss << funcDecl.value();

			ScopeIterator scope(funcInfo.scope);
			Result<string> funcBody = convertBody(iterator, funcInfo, metaData, scope, 1);
			if (funcBody.hasError)
				return funcBody.error;

			ss << funcBody.value();
		}
		else
		{
			return ErrorInfo("unexpected token in global, token: \'" + token + "\'", iterator.currentLine);
		}
	}

	return ss.str();
}
