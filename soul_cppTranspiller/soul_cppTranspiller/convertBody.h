#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "FuncNode.h"
#include "CurrentContext.h"

Result<BodyStatment_Result<SuperStatement>> convertBodyElement(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, SyntaxNodeId parentNode);
Result<BodyStatment_Result<SuperStatement>> convertBodyElement(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, uint32_t& openCurlyBracketCounter, SyntaxNodeId parentNode);
Result<std::shared_ptr<BodyNode>> convertBody(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context, SyntaxNodeId parentNode);
