#pragma once
#include "metaData.hpp"

Result<std::vector<Token>> tokenize(/*out*/ std::string& sourceFile, /*out*/ MetaData& metaData);


