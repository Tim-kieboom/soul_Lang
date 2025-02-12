#pragma once
#include "MetaData.h"
#include "DefineTemplateTypes.h"

Result<std::shared_ptr<DefineTemplateTypes>> getTemplateTypes(TokenIterator& iterator, CurrentContext& context);
