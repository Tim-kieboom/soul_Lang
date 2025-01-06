#include <sstream>
#include "ArgumentInfo.h"

using namespace std;

ArgumentType getArgumentType(const std::string& token)
{
	if (token == "out")
		return ArgumentType::out;

	if (token == "mut")
		return ArgumentType::mut;

	return ArgumentType::invalid;
}

std::string toString(ArgumentType type)
{
	switch (type)
	{
	case ArgumentType::default_:
		return "default";

	case ArgumentType::mut:
		return "mut";

	case ArgumentType::out:
		return "out";

	default:
		return "<invalid>";
	}
}

std::string toString(ArgumentInfo& arg)
{
	stringstream ss;
	if (arg.argType != ArgumentType::invalid && arg.argType != ArgumentType::default_)
		ss << toString(arg.argType) << ' ';
	
	ss << toString(arg.valueType) << ' ' << arg.argName;

	if (arg.isOptional)
		ss << ' ' << "= <value>";

	return ss.str();
}

std::string toString(std::vector<ArgumentInfo>& args)
{
	stringstream ss;
	for(uint64_t i = 0; i < args.size(); i++)
	{
		ss << toString(args.at(i));
		if(i++ >= args.size())
			ss << ", ";
	}
	return ss.str();
}
