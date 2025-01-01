#include "string_contain.h"
#include "string_split.h"
#include "string_find.h"

using namespace std;

bool string_contains(const string& str, const string& find)
{
	return str.find(find) != string::npos;
}

bool string_contains(const string& str, const char* find)
{
	return string_contains(str, string(find));
}

bool string_containsMultiple(const std::string& str, initializer_list<const char*> find)
{
	for (const char* findEl : find)
	{
		if (string_contains(str, string(findEl)))
			return true;
	}

	return false;
}

bool string_contains(const string& str, char find)
{
	return str.find(find) != string::npos;
}

bool string_contains(const std::string& str, initializer_list<char>& find)
{
	for (const char& ch : str)
	{
		if (initListEquals(find, ch))
			return true;
	}

	return false;
}

uint32_t string_count(const string& str, char findChar)
{
	uint32_t counter = 0;
	for (const char& ch : str)
	{
		if (ch == findChar)
			counter++;
	}

	return counter;
}

uint32_t string_count(const string& str, const char* findStr)
{
	return (uint32_t)string_find(str, findStr).size();
}


uint32_t string_count(const string& str, string findStr)
{
	return (uint32_t)string_find(str, findStr).size();
}
