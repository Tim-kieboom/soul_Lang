#pragma once
#include <unordered_map>

#include "Result.h"
#include "DuckType.h"
#include "ClassInfo.h"
#include "TypeWrapper.h"
#include "ComplexType.h"
#include "TokenIterator.hpp"
#include "equalTools.h"

std::string toString(const TypeInfo& type);

struct TypeInfo
{
	bool isArray = false;
	bool isPointer = false;
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
		//if (getDuckType(type) == DuckType::text)
		//	this->isMutable = false;
	}

	TypeInfo(ClassInfo* classInfo, bool isMutable = true)
		: isMutable(isMutable), complexType(ComplexType(classInfo))
	{
	}	

	Result<void> addTypeWrapper(TypeWrapper type, const uint64_t currentLine)
	{
		isArray = (type == TypeWrapper::array_);
		isPointer = (type == TypeWrapper::pointer);

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

	Result<TypeInfo> getTypeChild(uint64_t currentLine)
	{
		const uint64_t wrapSize = typeWrappers.size();
		if
		(
			(wrapSize == 0) ||
			(wrapSize == 1 && typeWrappers.at(wrapSize - 1) == TypeWrapper::refrence) ||
			(wrapSize == 2 && typeWrappers.at(wrapSize - 1) == TypeWrapper::refrence && typeWrappers.at(wrapSize - 2) == TypeWrapper::refrence)
		)
		{
			return ErrorInfo("can not get childType from type: \'" + toString(*this) + "\'", currentLine);
		}

		TypeInfo child = *this;
		child.typeWrappers.pop_back();

		TypeWrapper lastWrap = child.typeWrappers.back();
		child.isArray = (lastWrap == TypeWrapper::array_);
		child.isPointer = (lastWrap == TypeWrapper::pointer);

		return child;
	}

	Result<void> areTypesCompatiple(const TypeInfo& type, uint64_t currentLine) const
	{
		if (!isValid() || !type.isValid())
			return ErrorInfo("<invalid-type>: \'" + toString(*this) + "\'", currentLine);

		if (this->isComplexType != type.isComplexType)
			return ERROR_areTypesCompatiple_typeNotCompatiple(type, currentLine);

		if(type.isComplexType)
		{
			if(this->complexType.info.classInfo->className != type.complexType.info.classInfo->className)
				return ERROR_areTypesCompatiple_typeNotCompatiple(type, currentLine);
		}
		else
		{
			if (this->primType == PrimitiveType::compile_dynamic || type.primType == PrimitiveType::compile_dynamic)
				return {};

			if (getDuckType(this->primType) != getDuckType(type.primType))
				return ERROR_areTypesCompatiple_typeNotCompatiple(type, currentLine);
		}



		return {};
	}

	bool isValid() const
	{
		return !(primType == PrimitiveType::invalid && complexType.complexType_type == ComplexType_Type::invalid);
	}

	constexpr bool equals(const TypeInfo& other) const
	{
		bool boolEqs = isArray == other.isArray &&
					   isPointer == other.isPointer &&
					   isMutable == other.isMutable &&
					   refrenceCounter == refrenceCounter &&
					   isComplexType == other.isComplexType &&
					   vectorEquals(typeWrappers, other.typeWrappers);

		if (!boolEqs)
			return false;

		if (isComplexType)
			return complexType.equals(other.complexType);
		
		return primType == other.primType;
	}

private:
	ErrorInfo ERROR_areTypesCompatiple_typeNotCompatiple(const TypeInfo& type, uint64_t currentLine) const
	{
		return ErrorInfo("Type incompadible type1: \'" + toString(*this) + "\', type2 \'" + toString(type) + "\'", currentLine);
	}
};

Result<TypeInfo> getTypeInfo(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore);