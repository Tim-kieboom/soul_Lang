#pragma once
#include "MetaData.h"
#include "CurrentContext.h"

Result<std::string> soulToCpp_Type(RawType& type, MetaData& metaData, CurrentContext& contex);
