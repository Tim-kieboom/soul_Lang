#pragma once
#include <string>

struct VarInfo
{
	std::string name;
	std::string stringedRawType;
	bool isOption = false;

	VarInfo() = default;
	
	VarInfo(const std::string name, const std::string stringedRawType, bool isOption = false)
		: name(name), stringedRawType(stringedRawType), isOption(isOption)
	{
	}
};