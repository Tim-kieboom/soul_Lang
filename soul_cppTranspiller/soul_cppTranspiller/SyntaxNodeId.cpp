#include "SyntaxNodeId.h"

bool isSyntaxTree_Statment(SyntaxNodeId id)
{
	return id == SyntaxNodeId::FieldNode ||
		   id == SyntaxNodeId::Assignment ||
		   id == SyntaxNodeId::MethodeNode ||
		   id == SyntaxNodeId::ReturnStatment ||
		   id == SyntaxNodeId::InitializeVariable ||
		   id == SyntaxNodeId::FunctionCallStatment;
    
}

bool isSyntaxTree_Expression(SyntaxNodeId id)
{
	return id == SyntaxNodeId::Literal ||
		   id == SyntaxNodeId::Refrence ||
		   id == SyntaxNodeId::Variable ||
		   id == SyntaxNodeId::Optional ||
		   id == SyntaxNodeId::Increment ||
		   id == SyntaxNodeId::FunctionCall ||
		   id == SyntaxNodeId::StringLiteral ||
		   id == SyntaxNodeId::EmptyExpresion ||
		   id == SyntaxNodeId::BinairyExpression;
}
