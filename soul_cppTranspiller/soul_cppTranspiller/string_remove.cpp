#include "string_remove.h"
#include <vector>

#include "string_find.h"

using namespace std;

bool string_removeAt(/*out*/ string& str, int64_t index)
{
	index = (index < 0) ? str.size() + index : index;
	if ((uint64_t)index > str.size() - 1)
		return false;

	str.erase(index, index + 1);
	return true;
}

bool string_removeSpan(/*out*/ string& str, int64_t begin, int64_t end)
{
	if (begin == end)
		string_removeAt(str, begin);

	if (begin < 0)
		begin = str.size() + begin;

	if (end < 0)
	{
		if ((uint64_t)abs(end) + 2 > str.size())
			return false;

		end = str.size() + end + 1;
	}

	end = (end < 0) ? str.size() + end + 1 : end;
	if (end == 0 || begin > end || (uint64_t)end > str.size())
		return false;


	str.erase(str.begin() + begin, str.begin() + end);
	return true;
}

uint64_t string_remove(std::string& str, char ch)
{
	vector<uint32_t> removes = string_find(str, ch);
	str.erase(std::remove(str.begin(), str.end(), ch), str.end());
	return removes.size();
}

uint64_t string_remove(std::string& str, std::string removeStr)
{
	vector<uint32_t> removes = string_find(str, removeStr);
	for (auto index : removes)
		string_removeSpan(str, index, index + removeStr.size());

	return removes.size();
}

uint64_t string_remove(std::string& str, const char* removeStr)
{
	return string_remove(str, string(removeStr));
}