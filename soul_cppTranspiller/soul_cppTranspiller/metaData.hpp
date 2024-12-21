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
	std::unordered_map<std::string, std::vector<FuncInfo>> funcStore;
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

	void addFuncInfo(const std::string& funcName, const FuncInfo& info)
	{
		funcStore[funcName].push_back(info);
	}

	bool isFunction(const std::string& funcName)
	{
		return funcStore.find(funcName) != funcStore.end();
	}

	bool TryGetfuncInfo(const std::string& funcName, std::vector<ArgumentInfo> args, FuncInfo& funcInfo)
	{
		if (funcStore.find(funcName) == funcStore.end())
			return false;

		std::vector<FuncInfo>& funcs = funcStore.at(funcName);
		for (FuncInfo& func : funcs)
		{
			if (func.argsEquals(args))
			{
				funcInfo = func;
				return true;
			}
		}
		return false;
	}

	std::string addMethode(ClassInfo& classInfo, const FuncInfo& funcInfo, /*out*/MetaData& metaData)
	{
		std::stringstream ss;
		ss << classInfo.className << '#' << funcInfo.funcName;

		std::string methodeName = ss.str();
		classInfo.methodesNames.push_back(methodeName);
		return methodeName;
	}

	std::pair<std::string, std::string> addProperty(ClassInfo& classInfo, const std::string& varName, const FuncInfo& setter, const FuncInfo& getter, /*out*/MetaData& metaData)
	{
		std::stringstream ss;
		ss << classInfo.className << "#prop#" << varName << "#setter";
		std::string setterName = ss.str();

		ss << classInfo.className << "#prop#" << varName << "#getter";
		std::string getterName = ss.str();

		metaData.addFuncInfo(setterName, setter);
		metaData.addFuncInfo(getterName, getter);
		return { setterName, getterName };
	}
};