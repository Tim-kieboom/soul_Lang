#pragma once
#include <unordered_map>
#include "stringTools.h"
#include "C_strPair.h"
#include "tokens.h"

struct MetaData
{
	std::unordered_map<ui64_string, C_strPair> c_strStore;
};