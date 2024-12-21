#pragma once
#include "ClassInfo.h"

struct ClassInfo;
enum class ComplexType_Type
{
	invalid,
	classInfo
};

union ComplexType_Info
{
	ClassInfo* classInfo = nullptr;

	ComplexType_Info() {}
	~ComplexType_Info() {}

	constexpr bool equals(ComplexType_Type type, const ComplexType_Info& other) const
	{
		switch(type)
		{
		case ComplexType_Type::classInfo:
			return this->classInfo->equals(*other.classInfo);

		case ComplexType_Type::invalid:
			return true;

		default:
			return false;
		}
	}
};

struct ComplexType
{
	ComplexType_Type complexType_type = ComplexType_Type::invalid;
	ComplexType_Info info;

	ComplexType() = default;
	ComplexType(ClassInfo* classInfo)
	{
		info.classInfo = classInfo;
		complexType_type = ComplexType_Type::classInfo;
	}

	constexpr bool equals(const ComplexType& other) const
	{
		if (complexType_type != other.complexType_type)
			return false; //this is needed because YOU CAN NOT EQUAL diffrent union if there not the same type

		return info.equals(complexType_type, other.info);
	}
};