#include "string_replace.h"
#include "string_remove.h"
#include "string_find.h"

using namespace std;

uint32_t string_replace(/*out*/ string& str, char replaceChar, char withChar)
{
	uint32_t counter = 0;
	for (char& ch : str)
	{
		if (ch == replaceChar)
		{
			ch = withChar;
			counter++;
		}
	}

	return counter;
}

uint32_t string_replace(string& str, const string& replaceStr, const string& withStr)
{
	if (replaceStr.empty())
		return 0;

	uint32_t replaceCounter = 0;
	size_t pos = 0;
	while ((pos = str.find(replaceStr, pos)) != string::npos)
	{
		str.replace(pos, replaceStr.length(), withStr);
		pos += withStr.length();
		++replaceCounter;
	}

	return replaceCounter;
}

uint32_t string_replace(std::string& str, const char* replaceStr, const char* withStr)
{
	return string_replace(str, string(replaceStr), string(withStr));
}

uint32_t string_replace(std::string& str, const std::string& replaceStr, const char* withStr)
{
	return string_replace(str, replaceStr, string(withStr));
}

uint32_t string_replace(std::string& str, const char* replaceStr, const std::string& withStr)
{
	return string_replace(str, string(replaceStr), withStr);
}

uint32_t string_replace(std::string& str, char replaceChar, char withChar, uint64_t countLimit)
{
	uint32_t counter = 0;
	for (char& ch : str)
	{
		if (ch == replaceChar)
		{
			ch = withChar;
			counter++;
			if (counter == countLimit)
				break;
		}
	}

	return counter;
}

uint32_t string_replace(std::string& str, const std::string& replaceStr, const std::string& withStr, uint64_t countLimit)
{
	uint64_t replaceStrLength = replaceStr.size();

	uint32_t replaceCounter = 0;

	int64_t pos = 0;
	int64_t prevPos = 0;
	for (uint32_t i = 0; i < str.size(); i++)
	{
		pos = str.find(replaceStr, prevPos);
		prevPos = pos + replaceStrLength;

		if (pos < 0)
			break;

		str.replace(str.begin() + pos, str.begin() + prevPos, withStr.c_str());
		replaceCounter++;
		if (replaceCounter == countLimit)
			break;
	}

	return replaceCounter;
}

uint32_t string_replace(std::string& str, const char* replaceStr, const char* withStr, uint64_t countLimit)
{
	return string_replace(str, string(replaceStr), string(withStr), countLimit);
}

uint32_t string_replace(std::string& str, const std::string& replaceStr, const char* withStr, uint64_t countLimit)
{
	return string_replace(str, string(replaceStr), string(withStr), countLimit);
}

uint32_t string_replace(std::string& str, const char* replaceStr, const std::string& withStr, uint64_t countLimit)
{
	return string_replace(str, string(replaceStr), string(withStr), countLimit);
}

bool string_replaceAt(string& str, int64_t begin, int64_t end, const char* replaceStr)
{
	uint64_t strLength = strlen(replaceStr);

	if (!string_removeSpan(str, begin, end))
		return false;

	if (begin < 0)
		begin = str.size() + begin;

	string endStr = str.substr(begin);
	if (!string_removeSpan(str, begin, -1))
		return false;

	str.append(replaceStr);
	str.append(endStr);
	return true;
}

bool string_replaceAt(string& str, int64_t begin, int64_t end, const string& replaceStr)
{
	return string_replaceAt(str, begin, end, replaceStr.c_str());
}

bool string_replaceAt(string& str, int64_t index, const char* replaceStr)
{
	return string_replaceAt(str, index, index, replaceStr);
}

bool string_replaceAt(string& str, int64_t index, const string& replaceStr)
{
	return string_replaceAt(str, index, index, replaceStr.c_str());
}

bool string_replaceInSpan(std::string& str, const char* replaceStr, const char* withString, uint32_t begin, uint32_t end)
{
	uint64_t replaceStrLen = strlen(replaceStr);
	vector<uint32_t> indexes = string_find(str, replaceStr, begin);
	
	for(uint32_t index : indexes)
	{
		if (index > end)
			break;
		
		string_replaceAt(str, index, index + replaceStrLen, withString);
	}

	return true;
}
