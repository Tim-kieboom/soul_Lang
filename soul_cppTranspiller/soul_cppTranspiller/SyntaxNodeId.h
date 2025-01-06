#pragma once

enum class SyntaxNodeId
{
	SyntaxTree,

	Assignment,
	InitializeVariable,
	FuncNode,
	BodyNode,

	Variable,
	Literal,
	BinairyExpression,

	ClassNode,
	FieldNode,
	MethodeNode
};