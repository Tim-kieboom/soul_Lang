#include "string_copy.h"
#include <sstream>

using namespace std;

const char* string_copyTo_c_str(const string& str)
{
	char* buffer = new char[str.size() + 1];
	copy(str.begin(), str.end(), buffer);
	buffer[str.size()] = '\0';
	return buffer;
}

string concatToString(const vector<string>& vect)
{
	stringstream ss;
	for (const string& str : vect)
		ss << str;

	return ss.str();
}

string concatToString(const vector<string>& vect, const string& addBetweenStrs)
{
	stringstream ss;
	for (const string& str : vect)
		ss << str << addBetweenStrs;

	return ss.str();
}
