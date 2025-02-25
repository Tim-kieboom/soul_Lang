#pragma once
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "VarInfo.h"
#include "C_strPair.h"
#include "ClassInfo.h"
#include "CurrentContext.h"
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

		sortTemplateFunctionsLast(funcName);

		return funcs.at(funcIndex);
	}

	FuncDeclaration& addMethode(const std::string& methode, const ClassInfo& thisClass, const FuncDeclaration& funcInfo)
	{
		return addFunction(getMethodeMapEntryName(methode, thisClass), funcInfo);
	}

	bool tryGetC_str(const std::string& varName, std::string& value)
	{
		if (c_strStore.find(varName) == c_strStore.end())
			return false;

		value = c_strStore[varName].value;
		return true;
	}

	bool isFunction(const std::string& funcName, CurrentContext& context, bool* isFunctionInClass = nullptr)
	{
		if (context.inClass.hasValue() && isMethode(funcName, context.inClass.value()))
		{
			if(isFunctionInClass != nullptr)
				*isFunctionInClass = true;
				
			return true;
		}

		return funcStore.find(funcName) != funcStore.end();
	}

	bool isMethode(const std::string& methodeName, ClassInfo& thisClass)
	{
		return funcStore.find(getMethodeMapEntryName(methodeName, thisClass)) != funcStore.end();
	}

	bool isClass(const std::string& token)
	{
		return classStore.find(token) != classStore.end();
	}

	bool isClass(const RawType& type)
	{
		return classStore.find(type.getType_WithoutWrapper()) != classStore.end();
	}

	bool tryGetFunction(const std::string& funcName, CurrentContext& context, std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals, FuncDeclaration& funcInfo, int64_t currentLine, ErrorInfo& error, uint64_t* funcInfoIndex = nullptr)
	{
		bool isFunctionInClass = false;
		if (!isFunction(funcName, context, /*out*/&isFunctionInClass))
		{
			error = ErrorInfo("\'" + funcName + "\' is not a found function", currentLine);
			return false;
		}

		uint64_t i = 0;
		std::vector<FuncDeclaration>& funcs = (isFunctionInClass) ? funcStore.at(getMethodeMapEntryName(funcName, context.inClass.value())) : funcStore.at(funcName);
		for (FuncDeclaration& func : funcs)
		{
			if (func.argsCompatible(args, optionals, classStore, func.templateTypes, currentLine, error))
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

	bool tryGetCtor(const std::string& methodeName, ClassInfo& thisClass, CurrentContext& context, std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals, FuncDeclaration& funcInfo, int64_t currentLine, ErrorInfo& error, uint64_t* funcInfoIndex = nullptr)
	{
		return _tryGetMethode(methodeName, thisClass, context, args, optionals, funcInfo, currentLine, error, funcInfoIndex, true);
	}

	bool tryGetMethode(const std::string& methodeName, ClassInfo& thisClass, CurrentContext& context, std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals, FuncDeclaration& funcInfo, int64_t currentLine, ErrorInfo& error, uint64_t* funcInfoIndex = nullptr)
	{
		return _tryGetMethode(methodeName, thisClass, context, args, optionals, funcInfo, currentLine, error, funcInfoIndex, false);
	}

	std::string getMethodeMapEntryName(const std::string& methodeName, const ClassInfo& thisClass)
	{
		return thisClass.name + "#" + methodeName;
	}

private:
	bool _tryGetMethode(const std::string& methodeName, ClassInfo& thisClass, CurrentContext& context, std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals, FuncDeclaration& funcInfo, int64_t currentLine, ErrorInfo& error, uint64_t* funcInfoIndex, bool isCtor = false)
	{
		bool isValid = (isCtor) ? methodeName == thisClass.name : isMethode(methodeName, thisClass);

		if (!isValid)
		{
			error = ErrorInfo("\'" + methodeName + "\' is not a found methode for class: \'" + thisClass.name + "\'", currentLine);
			return false;
		}

		std::map<std::string, TemplateType> templates = context.currentTemplateTypes;
		for (auto& kv : thisClass.templateTypes)
			templates[kv.first] = kv.second;

		uint64_t i = 0;

		std::string classStoreKey = (isCtor) ? methodeName : getMethodeMapEntryName(methodeName, thisClass);
		
		std::vector<FuncDeclaration>& funcs = funcStore.at(classStoreKey);
		for (FuncDeclaration& func : funcs)
		{
			if (func.argsCompatible(args, optionals, classStore, templates, currentLine, error))
			{
				if (funcInfoIndex != nullptr)
					*funcInfoIndex = i;

				funcInfo = func;
				funcInfo.templateTypes = thisClass.templateTypes;
				return true;
			}
			i++;
		}

		if (funcInfoIndex != nullptr)
			*funcInfoIndex = i;

		return false;
	}

	void sortTemplateFunctionsLast(const std::string& funcName)
	{
		std::vector<FuncDeclaration>& funcs = funcStore[funcName];
		
		auto sortByLeastTemplates = [](const FuncDeclaration& one, const FuncDeclaration& two)
			{
				return one.templateTypes.size() < two.templateTypes.size();
			};

		std::sort(funcs.begin(), funcs.end(), sortByLeastTemplates);
	}

};