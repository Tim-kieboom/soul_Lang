#include "ArgumentInfo.h"
#include <sstream>

std::string toString(const ArgumentInfo& arg)
{
	std::stringstream ss;
	ss << '{' << toString(arg.argType) << ' ' << toString(arg.valueType) << ' ' << arg.name << '}';
	return ss.str();
}
