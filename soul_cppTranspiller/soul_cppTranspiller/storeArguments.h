#pragma once
#include <vector>

#include "Type.h"
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "ArgumentInfo.h"
#include "CurrentContext.h"

struct StoreArguments_Result
{
	std::vector<ArgumentInfo> args;
	std::vector<ArgumentInfo> optionals;

	StoreArguments_Result() = default;
	StoreArguments_Result(std::vector<ArgumentInfo>& args, std::vector<ArgumentInfo>& optionals)
		: args(args), optionals(optionals)
	{
	}
};

struct ClassArgumentInfo
{
	bool isCtor;
	ClassInfo& classInfo;

	ClassArgumentInfo(ClassInfo& classInfo, bool isCtor = false)
		: isCtor(isCtor), classInfo(classInfo)
	{
	}
};

Result<StoreArguments_Result> storeArguments(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, RawType& returnType, ClassArgumentInfo* inClass = nullptr);
