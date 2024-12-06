#pragma once
#include <unordered_map>

#include "Result.h"
#include "DuckType.h"
#include "ClassInfo.h"
#include "TypeWrapper.h"
#include "ComplexType.h"
#include "TokenIterator.hpp"

struct TypeInfo
{
	bool isArray = false;
	bool isMutable = true;
	bool isComplexType = true;

	ComplexType complexType;
	PrimitiveType primType = PrimitiveType::invalid;
	
	uint8_t refrenceCounter = 0;
	std::vector<TypeWrapper> typeWrappers;

	TypeInfo() = default;
	TypeInfo(PrimitiveType type, bool isMutable = true)
		: isMutable(isMutable), isComplexType(false), primType(type)
	{
	}

	TypeInfo(ClassInfo* classInfo, bool isMutable = true)
		: isMutable(isMutable), complexType(ComplexType(classInfo))
	{
	}	

	Result<void> addTypeWrapper(TypeWrapper type, const uint64_t currentLine)
	{
		isArray = (type == TypeWrapper::array_);

		if (type == TypeWrapper::array_ || type == TypeWrapper::pointer)
		{
			//i32&* or i32&[] is illigal
			if (refrenceCounter != 0)
				return ErrorInfo("can not have pointer or array after refrence", currentLine);
		}

		if(type == TypeWrapper::refrence)
		{
			//i32&& is oke i32&&& is illigal
			refrenceCounter++;
			if (refrenceCounter > 2)
				return ErrorInfo("can only have 2 refrences per type", currentLine);
		}

		typeWrappers.push_back(type);
		return {};
	}

	bool isValid() const
	{
		return !(primType == PrimitiveType::invalid && complexType.complexType_type == ComplexType_Type::invalid);
	}
};

std::string toString(TypeInfo& type);
Result<TypeInfo> getTypeInfo(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore);