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
};