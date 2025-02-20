#include "SyntaxNodeId.h"

bool isSyntaxTree_Statment(SyntaxNodeId id)
{
	return	id == SyntaxNodeId::FieldNode ||
			id == SyntaxNodeId::Assignment ||
			id == SyntaxNodeId::IfStatment ||
			id == SyntaxNodeId::ForStatment ||
			id == SyntaxNodeId::MethodeNode ||
			id == SyntaxNodeId::ElseStatment ||
			id == SyntaxNodeId::WhileStatment ||
			id == SyntaxNodeId::EmptyStatment ||
			id == SyntaxNodeId::BreakStatment ||
			id == SyntaxNodeId::ReturnStatment ||
			id == SyntaxNodeId::ElseIfStatment ||
			id == SyntaxNodeId::ContinueStatment ||
			id == SyntaxNodeId::InitializeVariable ||
			id == SyntaxNodeId::FunctionCallStatment ||
			id == SyntaxNodeId::CompileConstVariable;
}

bool isSyntaxTree_Expression(SyntaxNodeId id)
{
	return	id == SyntaxNodeId::Literal ||
			id == SyntaxNodeId::Variable ||
			id == SyntaxNodeId::Refrence ||
			id == SyntaxNodeId::Increment ||
			id == SyntaxNodeId::IndexArray ||
			id == SyntaxNodeId::FunctionCall ||
			id == SyntaxNodeId::StringLiteral ||
			id == SyntaxNodeId::EmptyExpresion ||
			id == SyntaxNodeId::ConstructArray ||
			id == SyntaxNodeId::CopyExpression ||
			id == SyntaxNodeId::RangeExpression ||
			id == SyntaxNodeId::MemberExpression ||
			id == SyntaxNodeId::BinairyExpression ||
			id == SyntaxNodeId::DefineTemplateTypes;
}
