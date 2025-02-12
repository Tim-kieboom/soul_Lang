#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"

Result<std::vector<Token>> tokenize(/*out*/ std::string& sourceFile, /*out*/ MetaData& metaData);


