#pragma once

enum class SyntaxNodeId
{
	SyntaxTree,

	/*funcNodes*/
	Assignment,
	InitializeVariable,
	FuncNode,
	BodyNode,
	FunctionCallStatment,

	/*expressions*/
	Variable,
	Literal,
	BinairyExpression,
	Increment,
	NoExpression,
	FunctionCall,
	EmptyExpresion,

	/*classNodes*/
	ClassNode,
	FieldNode,
	MethodeNode
};