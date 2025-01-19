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
	FunctionCallStatment,
	CompileConstVariable,

	/*expressions*/
	Variable,
	Literal,
	Optional,
	Refrence,
	Increment,
	FunctionCall,
	StringLiteral,
	EmptyExpresion,
	BinairyExpression
};

bool isSyntaxTree_Statment(SyntaxNodeId id);
bool isSyntaxTree_Expression(SyntaxNodeId id);
