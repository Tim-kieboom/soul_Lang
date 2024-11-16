#include "string_find.h"

using namespace std;

vector<uint32_t> string_find(const string& str, char findChar, uint64_t offset)
{
	vector<uint32_t> finds;
	uint32_t counter = 0;
	for (uint64_t i = offset; i < str.size(); i++)
	{
		if (str.at(i) == findChar)
		{
			finds.push_back(i);
		}
	}

	return finds;
}

vector<uint32_t> string_find(const string& str, string findStr, uint64_t offset)
{
	const uint64_t findStrSize = findStr.size();

	vector<uint32_t> finds;
	uint64_t prevPos = offset;
	uint64_t pos = 0;
	for (uint64_t i = 0; i < str.size(); i++)
	{
		if ((pos = str.find(findStr, prevPos)) == string::npos)
			return finds;

		finds.push_back((uint32_t)pos);
		prevPos = pos + findStrSize;
	}

	return {};
}

vector<uint32_t> string_find(const string& str, const char* findStr, uint64_t offset)
{
	return string_find(str, string(findStr), offset);
}

int64_t string_findFirst(const std::string& str, char findChar, uint64_t offset)
{
	vector<uint32_t> result = string_find(str, findChar, offset);
	if (result.empty())
		return -1;

	return result.at(0);
}

int64_t string_findFirst(const std::string& str, std::string findStr, uint64_t offset)
{
	vector<uint32_t> result = string_find(str, findStr, offset);
	if (result.empty())
		return -1;

	return result.at(0);
}