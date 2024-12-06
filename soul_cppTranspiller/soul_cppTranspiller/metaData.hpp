#pragma once
#include <sstream>
#include <unordered_map>

#include "Result.h"
#include "FuncInfo.h"
#include "C_strPair.h"
#include "TokenIterator.hpp"
#include "TransPillerOption.h"

struct MetaData
{
	TranspilerOptions transpillerOption;
	std::vector<VarInfo> globalScope;
	std::unordered_map<std::string, FuncInfo> funcStore;
	std::unordered_map<std::string, C_strPair> c_strStore;
	std::unordered_map<std::string, ClassInfo> classStore;
	std::unordered_map<std::string, std::string> cppIncludeStore;

	MetaData() = default;

	void addToGlobalScope(const VarInfo& varInfo)
	{
		globalScope.push_back(varInfo);
	}

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