#pragma once
#include <vector> 
#include <sstream> 
#include <unordered_map>

#include "tokens.h"
#include "FuncInfo.h"
#include "C_strPair.h"
#include "stringTools.h"
struct MetaData
{
	std::vector<std::vector<VarInfo>> scopeStack;
	std::unordered_map<std::string, FuncInfo> funcStore;
	std::unordered_map<std::string, C_strPair> c_strStore;
	std::unordered_map<std::string, std::string> cppIncludeStore;

	void addCppInclude(const std::string& key, std::string value)
	{
		cppIncludeStore[key] = value;
	}

	std::string getCpptIncludes()
	{
		std::stringstream ss;
		for (auto& pair : cppIncludeStore)
			ss << pair.second << "\n";
		return ss.str();
	}

	void addFuncInfo(const std::string& funcName, FuncInfo info)
	{
		funcStore[funcName] = info;
	}

	bool TryGetfuncInfo(const std::string& funcName, FuncInfo& funcInfo)
	{
		if (funcStore.find(funcName) != funcStore.end())
		{
			funcInfo = funcStore.at(funcName);
			return true;
		}

		return false;
	}
};