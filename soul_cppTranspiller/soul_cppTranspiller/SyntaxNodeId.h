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
	FunctionCall,
	ReturnStatment,
	EmptyExpresion,
	Optional,

	/*classNodes*/
	ClassNode,
	FieldNode,
	MethodeNode
};