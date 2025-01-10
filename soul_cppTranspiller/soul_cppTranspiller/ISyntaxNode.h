#pragma once
#include <iostream> 
#include <sstream> 
#include <vector> 
#include "SyntaxNodeId.h"

class ISyntaxNode
{
public:
	virtual ~ISyntaxNode() = default;
	virtual void print() const = 0;
	virtual std::string printToString() const = 0;
	virtual SyntaxNodeId getId() const = 0;
};