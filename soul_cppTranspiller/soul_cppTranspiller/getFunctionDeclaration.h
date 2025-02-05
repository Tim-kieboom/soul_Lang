#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "FuncDeclaration.h"

Result<FuncDeclaration> getFunctionDeclaration(TokenIterator& iterator, MetaData& metaData, bool isForwardDeclared, ClassInfo* currentClass = nullptr);
