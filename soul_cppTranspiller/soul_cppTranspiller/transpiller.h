#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "MetaData.hpp"

Result<std::string> transpileToCpp(const std::vector<Token> tokens, MetaData& metaData);