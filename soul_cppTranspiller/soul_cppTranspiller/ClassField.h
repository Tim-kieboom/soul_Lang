#pragma once
#include "varInfo.h"

enum class FieldType
{
	property,
	private_
};

struct Field
{
	VarInfo varInfo;
	FieldType type = FieldType::private_;

	Field() = default;
	Field(VarInfo& varInfo, FieldType type)
		: varInfo(varInfo), type(type)
	{
	}
};