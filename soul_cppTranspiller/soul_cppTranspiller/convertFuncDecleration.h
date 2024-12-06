#pragma once
#include "MetaData.hpp"

Result<std::string> convertFuncDeclaration(/*out*/TokenIterator& iterator, /*out*/MetaData& metaData, /*out*/FuncInfo& funcInfo);
