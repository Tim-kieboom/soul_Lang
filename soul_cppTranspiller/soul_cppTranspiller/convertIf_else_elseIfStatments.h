#pragma once
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "CurrentContext.h"
#include "FuncDeclaration.h"
#include "If_else_elseIf_Statments.h"

Result<BodyStatment_Result<SuperConditionalStatment>> convertElseStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);
Result<BodyStatment_Result<SuperConditionalStatment>> convertElseIfStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);
Result<BodyStatment_Result<SuperConditionalStatment>> convertIfStatment(TokenIterator& iterator, MetaData& metaData, FuncDeclaration& funcInfo, CurrentContext& context);

