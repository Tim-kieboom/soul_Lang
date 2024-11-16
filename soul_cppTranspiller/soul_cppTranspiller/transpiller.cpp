#include "transpiller.h"
#include <sstream>

#include "funcBody.h"
#include "TokenIterator.h"
#include "funcDecleration.h"
#include "internalFunction.h"

using namespace std;

static void addConstStrings_ToStream(/*out*/ stringstream& fileStream, const MetaData& metaData)
{
	fileStream << "\n";
	for (const auto& keyValue : metaData.c_strStore)
	{
		const C_strPair& c_str = keyValue.second;
		fileStream << "constexpr const char* " << c_str.name << " = " << c_str.value << ";\n";
	}
	fileStream << "\n";
}

static void addInternalFunctions_ToMetaData(/*out*/ MetaData& metaData)
{
	for(const FuncInfo& funcInfo : internalFunctions)
	{
		metaData.addFuncInfo(string(funcInfo.funcName), funcInfo);
	}
}

Result<string> transpileToCpp(const vector<Token> tokens, const TranspilerOptions& option, MetaData& metaData)
{
	metaData.addCppInclude("utility", "#include <utility>");
	metaData.addCppInclude("cstdint", "#include <cstdint>");
	metaData.addCppInclude("assert.h", "#include <assert.h>");
	metaData.addCppInclude("iostream", "#include <iostream>");
	metaData.addCppInclude("stdexcept", "#include <stdexcept>");
	addInternalFunctions_ToMetaData(/*out*/metaData);

	stringstream fileStream;

	addConstStrings_ToStream(/*out*/fileStream, metaData);

	bool validEnd = true;

	string token;
	TokenIterator iterator(tokens);

	while (iterator.nextToken(/*out*/token))
	{
		if (token == "func")
		{
			validEnd = false;

			FuncInfo funcInfo;
			Result<string> funcDecl = convertFuncDeclaration(/*out*/iterator, /*out*/metaData, /*out*/funcInfo);
			if (funcDecl.hasError)
				return funcDecl;
				
			fileStream << funcDecl.value();

			Result<string> funcBody = convertFunctionBody(/*out*/iterator, /*out*/funcInfo, /*out*/metaData);
			if (funcBody.hasError)
				return funcBody;

			fileStream << funcBody.value();
			validEnd = true;
		}

		break;
	}

	if (!validEnd)
		return ErrorInfo("unexpected end of line", iterator.currentLine);

	return fileStream.str();
}
