#include "string_split.h"

using namespace std;

bool initListEquals(const initializer_list<char>& splitOnChar, char ch)
{
	for (char el : splitOnChar)
	{
		if (ch == el)
			return true;
	}
	return false;
}

bool initListEquals(const initializer_list<const char*> splitOnChar, const string& ch)
{
	for (const char* el : splitOnChar)
	{
		if (ch == el)
			return true;
	}
	return false;
}

vector<string> string_split(const string& str, char splitOnChar)
{
	vector<string> splits;
	splits.reserve(20);

	string strCopy = str;
	uint64_t size = str.size();
	for (uint64_t i = 0; i < size; i++)
	{
		char ch = strCopy.at(i);

		if (ch == splitOnChar)
		{
			splits.push_back(strCopy.substr(0, i));
			strCopy.erase(0, i + 1);
			i = 0;
			size = strCopy.size();
		}
	}

	splits.push_back(strCopy);
	return splits;
}

vector<string> string_split(const string& str, const string& splitOnString)
{
	uint64_t splitStrLenght = splitOnString.size();
	vector<string> splits;
	splits.reserve(20);

	int64_t pos = 0;
	int64_t prevPos = 0;
	while (true)
	{
		pos = str.find(splitOnString, prevPos);

		if (pos < 0)
			break;

		splits.push_back(str.substr(prevPos, pos));
		prevPos = pos + splitStrLenght;
	}

	splits.push_back(str.substr(prevPos, str.size() - 1));
	return splits;
}

vector<string> string_split(const string& str, const char* _splitOnString)
{
	string splitOnString = string(_splitOnString);
	return string_split(str, splitOnString);
}

vector<string> string_split(const string& str, initializer_list<char> splitOnString)
{
	vector<string> splits;
	splits.reserve(20);

	string strCopy = str;
	uint64_t size = str.size();
	for (uint64_t i = 0; i < size; i++)
	{
		char ch = strCopy.at(i);

		if (initListEquals(splitOnString, ch))
		{
			splits.push_back(strCopy.substr(0, i));
			strCopy.erase(0, i + 1);
			i = 0;
			size = strCopy.size();
		}
	}

	splits.push_back(strCopy);
	return splits;
}

vector<string> string_split(const string& str, initializer_list<string> splitOnStrings)
{
	vector<string> splits;
	splits.reserve(20);

	size_t prevPos = 0;
	size_t pos = string::npos;

	while (prevPos < str.length())
	{
		pos = string::npos;
		string matchedSplitStr;

		// Find the earliest occurrence of any split string
		for (const auto& splitStr : splitOnStrings)
		{
			size_t foundPos = str.find(splitStr, prevPos);
			if (foundPos != string::npos && (foundPos < pos || pos == string::npos))
			{
				pos = foundPos;
				matchedSplitStr = splitStr;
			}
		}

		if (pos == string::npos)
		{
			// No more occurrences of any split string, add the rest of the string
			splits.push_back(str.substr(prevPos));
			break;
		}

		// Add the substring before the split string
		splits.push_back(str.substr(prevPos, pos - prevPos));
		prevPos = pos + matchedSplitStr.length();
	}

	return splits;
}

vector<string> string_split(const string& str, initializer_list<const char*> splitOnStrings)
{
	vector<string> splits;
	splits.reserve(20);

	size_t prevPos = 0;
	size_t pos = string::npos;

	while (prevPos < str.length())
	{
		pos = string::npos;
		string matchedSplitStr;

		// Find the earliest occurrence of any split string
		for (const auto& splitStr : splitOnStrings)
		{
			size_t foundPos = str.find(splitStr, prevPos);
			if (foundPos != string::npos && (foundPos < pos || pos == string::npos))
			{
				pos = foundPos;
				matchedSplitStr = splitStr;
			}
		}

		if (pos == string::npos)
		{
			// No more occurrences of any split string, add the rest of the string
			splits.push_back(str.substr(prevPos));
			break;
		}

		// Add the substring before the split string
		splits.push_back(str.substr(prevPos, pos - prevPos));
		prevPos = pos + matchedSplitStr.length();
	}

	return splits;
}

std::vector<std::string> string_splitOn(const std::string& str, char splitOnChar)
{
	vector<string> splits;
	splits.reserve(20);

	string strCopy = str;
	uint64_t size = str.size();
	for (uint64_t i = 0; i < size; i++)
	{
		char ch = strCopy.at(i);

		if (ch == splitOnChar)
		{
			splits.push_back(strCopy.substr(0, i));
			splits.push_back(string() + strCopy.at(i));
			strCopy.erase(0, i + 1);
			i = 0;
			size = strCopy.size();
		}
	}

	splits.push_back(strCopy);
	return splits;
}

std::vector<std::string> string_splitOn(const std::string& str, const std::string& splitOnString)
{
	uint64_t splitStrLenght = splitOnString.size();
	vector<string> splits;
	splits.reserve(20);

	int64_t pos = 0;
	int64_t prevPos = 0;
	while (true)
	{
		pos = str.find(splitOnString, prevPos);

		if (pos < 0)
			break;

		splits.push_back(str.substr(prevPos, pos));
		splits.push_back(splitOnString);
		prevPos = pos + splitStrLenght;
	}

	splits.push_back(str.substr(prevPos, str.size() - 1));
	return splits;
}

std::vector<std::string> string_splitOn(const std::string& str, const char* splitOnString)
{
	return string_splitOn(str, string(splitOnString));
}

std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<char> splitOnChars)
{
	vector<string> splits;
	splits.reserve(20);

	string strCopy = str;
	uint64_t size = str.size();
	for (uint64_t i = 0; i < size; i++)
	{
		char ch = strCopy.at(i);

		if (initListEquals(splitOnChars, ch))
		{
			splits.push_back(strCopy.substr(0, i));
			splits.push_back(string() + strCopy.at(i));
			strCopy.erase(0, i + 1);
			i = 0;
			size = strCopy.size();
		}
	}

	splits.push_back(strCopy);
	return splits;
}

std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<string> splitOnChars)
{
	std::vector<std::string> splits;
	splits.reserve(40);  // Increased reserve size to account for split strings

	size_t prevPos = 0;
	size_t pos = std::string::npos;

	while (prevPos < str.length())
	{
		pos = std::string::npos;
		std::string matchedSplitStr;

		// Find the earliest occurrence of any split string
		for (const auto& splitStr : splitOnChars)
		{
			size_t foundPos = str.find(splitStr, prevPos);
			if (foundPos != std::string::npos && (foundPos < pos || pos == std::string::npos))
			{
				pos = foundPos;
				matchedSplitStr = splitStr;
			}
		}

		if (pos == std::string::npos)
		{
			// No more occurrences of any split string, add the rest of the string
			splits.push_back(str.substr(prevPos));
			break;
		}

		// Add the substring before the split string
		splits.push_back(str.substr(prevPos, pos - prevPos));

		// Add the matched split string
		splits.push_back(matchedSplitStr);

		prevPos = pos + matchedSplitStr.length();
	}

	return splits;
}

std::vector<std::string> string_splitOn(const std::string& str, std::initializer_list<const char*> splitOnChars)
{
	std::vector<std::string> splits;
	splits.reserve(40);  // Increased reserve size to account for split strings

	size_t prevPos = 0;
	size_t pos = std::string::npos;

	while (prevPos < str.length())
	{
		pos = std::string::npos;
		std::string matchedSplitStr;

		// Find the earliest occurrence of any split string
		for (const auto& splitStr : splitOnChars)
		{
			size_t foundPos = str.find(splitStr, prevPos);
			if (foundPos != std::string::npos && (foundPos < pos || pos == std::string::npos))
			{
				pos = foundPos;
				matchedSplitStr = splitStr;
			}
		}

		if (pos == std::string::npos)
		{
			// No more occurrences of any split string, add the rest of the string
			splits.push_back(str.substr(prevPos));
			break;
		}

		// Add the substring before the split string
		splits.push_back(str.substr(prevPos, pos - prevPos));

		// Add the matched split string
		splits.push_back(matchedSplitStr);

		prevPos = pos + matchedSplitStr.length();
	}

	return splits;
}