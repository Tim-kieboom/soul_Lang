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
	StringLiteral,
	BinairyExpression,
	Increment,
	FunctionCall,
	ReturnStatment,
	EmptyExpresion,
	Optional,

	/*classNodes*/
	ClassNode,
	FieldNode,
	MethodeNode
};