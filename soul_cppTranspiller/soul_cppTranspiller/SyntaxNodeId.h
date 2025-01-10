#pragma once

enum class SyntaxNodeId
{
	SyntaxTree,

	/*funcNodes*/
	Assignment,
	InitializeVariable,
	FuncNode,
	BodyNode,

	/*expressions*/
	Variable,
	Literal,
	BinairyExpression,
	Increment,
	NoExpression,

	/*classNodes*/
	ClassNode,
	FieldNode,
	MethodeNode
};