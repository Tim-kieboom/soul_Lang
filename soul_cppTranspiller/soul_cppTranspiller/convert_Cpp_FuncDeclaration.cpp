#include "convert_Cpp_FuncDeclaration.h"
#include "soulToCpp.Type.h"

using namespace std;

static inline Result<void> convertCppArgument(ArgumentInfo& arg, MetaData& metaData, stringstream& ss)
{
	Result<string> cppType = soulToCpp_Type(arg.valueType, metaData);
	if (cppType.hasError)
		return cppType.error;

	ss << cppType.value();

	if (arg.argType == ArgumentType::out)
		ss << '&';

	ss << ' ' << arg.argName;
	return {};
}

Result<string> convert_Cpp_FuncDeclaration(FuncDeclaration& funcInfo, MetaData& metaData)
{
	if (funcInfo.functionName == "main" && !funcInfo.args.empty())
		return string("int main(int __Soul_argc__, char** __Soul_argv__)");

	stringstream ss;
	Result<RawType> returnTypeResult = getRawType_fromStringedRawType(funcInfo.returnType, metaData.classStore, 0);
	if (returnTypeResult.hasError)
		return returnTypeResult.error;

	Result<string> cppReturnType = soulToCpp_Type(returnTypeResult.value(), metaData);
	if (cppReturnType.hasError)
		return cppReturnType.error;

	ss << cppReturnType.value() << ' ' << funcInfo.functionName << '(';
	for (uint32_t i = 0; i < funcInfo.args.size(); i++)
	{
		Result<void> result = convertCppArgument(funcInfo.args[i], metaData, /*out*/ss);
		if (result.hasError)
			return result.error;

		if (i < funcInfo.args.size()-1)
			ss << ", ";

	}

	if (!funcInfo.args.empty() && !funcInfo.optionals.empty())
		ss << ", ";

	vector<ArgumentInfo> optionals(funcInfo.optionals.size());
	for (auto& option : funcInfo.optionals)
	{
		optionals.at(option.second.argPosition) = option.second;
	}

	for (uint32_t i = 0; i < optionals.size(); i++)
	{
		Result<void> result = convertCppArgument(optionals[i], metaData, /*out*/ss);
		if (result.hasError)
			return result.error;

		if (i < funcInfo.args.size()-1)
			ss << ", ";
	}

	ss << ')';
	return ss.str();
}
