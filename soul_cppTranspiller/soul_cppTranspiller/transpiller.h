#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "tokenizer.h"
#include "Result.h"

struct TranspilerOptions
{
	int not_yet_implemented;
};

Result<std::string> transpileToCpp(const std::vector<Token> tokens, const TranspilerOptions& option, MetaData& metaData);
