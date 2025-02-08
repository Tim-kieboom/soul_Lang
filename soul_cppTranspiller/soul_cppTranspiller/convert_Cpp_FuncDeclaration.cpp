#include "convert_Cpp_FuncDeclaration.h"
#include "soulToCpp.Type.h"
#include "stringTools.h"

using namespace std;

static inline Result<void> convertCppArgument(ArgumentInfo& arg, MetaData& metaData, stringstream& ss, Nullable<string>& ctorArg, CurrentContext& dummyContext)
{
	Result<string> cppType = soulToCpp_Type(arg.valueType, metaData, dummyContext);
	if (cppType.hasError)
		return cppType.error;

	ss << cppType.value();

	if (arg.argType == ArgumentType::out)
		ss << '&';

	string name = arg.argName;
	if(string_contains(arg.argName, "this."))
	{
		name = string_split(arg.argName, ".").at(1);

		ctorArg = Nullable<string>(name + "(" + name + ")");
	}

	ss << ' ' << name;
	return {};
}

static inline bool isFuncCtor(FuncDeclaration& funcInfo)
{
	vector<string> splits = string_split(funcInfo.functionName, { "#" });
	if (splits.size() > 1)
		return (splits.at(1) == "ctor");
	
	return false;
}

Result<string> convert_Cpp_FuncDeclaration(FuncDeclaration& funcInfo, MetaData& metaData, unordered_set<string>& templateTypes, bool* isMethode)
{
	if (funcInfo.functionName == "main" && !funcInfo.args.empty())
		return string("int main(int __Soul_argc__, char** __Soul_argv__)");

	stringstream ss;
	stringstream ctorArgs;

	CurrentContext dummyContext = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>()));
	dummyContext.currentTemplateTypes = templateTypes;

	if (isMethode != nullptr)
		*isMethode = string_contains(funcInfo.functionName, '#');

	if (funcInfo.isConstexpr)
		ss << "constexpr ";

	if (!isFuncCtor(funcInfo))
	{
		Result<RawType> returnTypeResult = getRawType_fromStringedRawType(funcInfo.returnType, metaData.classStore, templateTypes, 0);
		if (returnTypeResult.hasError)
			return returnTypeResult.error;

		Result<string> cppReturnType = soulToCpp_Type(returnTypeResult.value(), metaData, dummyContext);
		if (cppReturnType.hasError)
			return cppReturnType.error;

		ss << cppReturnType.value() << ' ' << string_split(funcInfo.functionName, { "#" }).back() << '(';
	}
	else
	{
		ss << string_split(funcInfo.functionName, { "#" }).at(0) << '(';
	}


	for (uint32_t i = 0; i < funcInfo.args.size(); i++)
	{
		Nullable<string> ctorArg;
		Result<void> result = convertCppArgument(funcInfo.args[i], metaData, /*out*/ss, /*out*/ctorArg, dummyContext);
		if (result.hasError)
			return result.error;

		if (ctorArg.hasValue())
		{
			if(ctorArgs.str().size() == 0)
			{
				ctorArgs << ':' << ctorArg.value();
			}
			else
			{
				ctorArgs << ',' << ctorArg.value();

			}
		}

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
		Nullable<string> ctorArg;
		Result<void> result = convertCppArgument(optionals[i], metaData, /*out*/ss, /*out*/ctorArg, dummyContext);
		if (result.hasError)
			return result.error;

		if (ctorArg.hasValue())
		{
			if (ctorArgs.str().size() == 0)
			{
				ctorArgs << ':' << ctorArg.value();
			}
			else
			{
				ctorArgs << ',' << ctorArg.value();

			}
		}

		if (i < funcInfo.args.size()-1)
			ss << ", ";
	}

	ss << ')' << ctorArgs.str();
	return ss.str();
}
