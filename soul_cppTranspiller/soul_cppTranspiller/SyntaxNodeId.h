#pragma once

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
	RawArrayToSoulArray,
	FunctionCallStatment,
	CompileConstVariable,

	/*expressions*/
	Literal,
	Variable,
	Refrence,
	Increment,
	IndexArray,
	FunctionCall,
	StringLiteral,
	EmptyExpresion,
	ConstructArray,
	CopyExpression,
	RangeExpression,
	MemberExpression,
	BinairyExpression,
	DefineTemplateTypes,

	invalid
};

bool isSyntaxTree_Statment(SyntaxNodeId id);
bool isSyntaxTree_Expression(SyntaxNodeId id);
