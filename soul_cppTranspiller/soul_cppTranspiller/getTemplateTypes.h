#pragma once
#include "MetaData.h"
#include "TemplateTypes.h"

Result<std::shared_ptr<TemplateTypes>> getTemplateTypes(TokenIterator& iterator, CurrentContext& context);
