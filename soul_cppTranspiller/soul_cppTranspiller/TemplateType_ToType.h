#pragma once
#include "TemplateType.h"
#include "Type.h"

struct TemplateType_ToType
{
	TemplateType templateType;
	RawType type;

	TemplateType_ToType() = default;
	TemplateType_ToType(TemplateType& templateType, RawType& type)
		: templateType(templateType), type(type)
	{
	}
};