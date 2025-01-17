#pragma once
#include <sstream>
#include <unordered_map>

#include "VarInfo.h"
#include "C_strPair.h"
#include "ClassInfo.h"
#include "FuncDeclaration.h"

class MetaData
{
public:
	std::vector<VarInfo> globalScope;
	std::unordered_map<std::string, C_strPair> c_strStore;
	std::unordered_map<std::string, ClassInfo> classStore;
	std::unordered_map<std::string, std::vector<FuncDeclaration>> funcStore;

	MetaData() = default;

	void addToGlobalScope(const VarInfo& varInfo)
	{
		globalScope.push_back(varInfo);
	}

	void addC_str(std::string& varName, std::string& value)
	{
		c_strStore[varName] = C_strPair(varName, value);
	}

	FuncDeclaration& addFunction(const std::string& funcName, const FuncDeclaration& funcInfo)
	{
		std::vector<FuncDeclaration>& funcs = funcStore[funcName];
		
		uint64_t funcIndex = funcs.size();
		funcs.push_back(funcInfo);
		return funcs.at(funcIndex);
	}

	bool tryGetC_str(const std::string& varName, std::string& value)
	{
		if (c_strStore.find(varName) == c_strStore.end())
			return false;

		value = c_strStore[varName].value;
		return true;
	}

	bool isFunction(const std::string& funcName)
	{
		return funcStore.find(funcName) != funcStore.end();
	}

	bool isClass(const std::string& token)
	{
		return classStore.find(token) != classStore.end();
	}

	bool isClass(const RawType& type)
	{
		return classStore.find(type.getType_WithoutWrapper()) != classStore.end();
	}

	bool tryGetFuncInfo(const std::string& funcName, std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals, FuncDeclaration& funcInfo, int64_t currentLine, ErrorInfo& error, uint64_t* funcInfoIndex = nullptr)
	{
		if (funcStore.find(funcName) == funcStore.end())
			return false;

		uint64_t i = 0;
		std::vector<FuncDeclaration>& funcs = funcStore.at(funcName);
		for (FuncDeclaration& func : funcs)
		{
			if (func.argsCompatible(args, optionals, classStore, currentLine, error))
			{
				if (funcInfoIndex != nullptr)
					*funcInfoIndex = i;

				funcInfo = func;
				return true;
			}
			i++;
		}

		if (funcInfoIndex != nullptr)
			*funcInfoIndex = i;

		return false;
	}

};