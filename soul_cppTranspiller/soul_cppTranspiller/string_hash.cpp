#include "string_hash.h"
using namespace std;

uint64_t string_hash(const string& str)
{
	return string_consexprHash(str.c_str());
}

uint64_t string_hash(const char* str)
{
	return string_consexprHash(str);
}