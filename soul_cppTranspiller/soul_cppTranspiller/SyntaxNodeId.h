#pragma once

enum class SyntaxNodeId_Statment
{
	/*Statment*/
	FieldNode,
	Assignment,
	IfStatment,
	ForStatment,
	MethodeNode,
	ElseStatment,
	WhileStatment,
	EmptyStatment,
	BreakStatment,
	ReturnStatment,
	ElseIfStatment,
	ContinueStatment,
	InitializeVariable,
	FunctionCallStatment,
	CompileConstVariable,
};

enum class SyntaxNodeId
{
	SyntaxTree,
	FuncNode,
	BodyNode,
	ClassNode,

	/*Statment*/
	FieldNode,
	Assignment,
	IfStatment,
	ForStatment,
	MethodeNode,
	ElseStatment,
	WhileStatment,
	EmptyStatment,
	BreakStatment,
	ReturnStatment,
	ElseIfStatment,
	ContinueStatment,
	InitializeVariable,
	FunctionCallStatment,
	CompileConstVariable,

	/*expressions*/
	Literal,
	Variable,
	Optional,
	Refrence,
	Increment,
	IndexArray,
	FunctionCall,
	StringLiteral,
	EmptyExpresion,
	ConstructArray,
	CopyExpression,
	RangeExpression,
	BinairyExpression,

	invalid
};

bool isSyntaxTree_Statment(SyntaxNodeId id);
bool isSyntaxTree_Expression(SyntaxNodeId id);
