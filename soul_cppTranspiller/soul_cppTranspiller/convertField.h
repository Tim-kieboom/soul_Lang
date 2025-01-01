#pragma once
#include "MetaData.hpp"
#include "ScopeIterator.h"

Result<std::string> convertField(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, TypeInfo& type, ScopeIterator& classScope, ClassAccessType& classAccess);
