#include "soulChecker.h"
#include "stringTools.h" 

bool checkName(const std::string& name)
{
	if (name.empty())
		return false;

	//check if first char is number
	if (initListEquals(charNumbers, name.at(0)))
		return false;

	if (initListEquals(illigalNames, name))
		return false;

	return !string_contains(name, illigalNameSymbols);
}