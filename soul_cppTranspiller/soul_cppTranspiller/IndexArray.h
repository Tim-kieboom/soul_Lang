#pragma once
#include "SuperExpression.h"

class IndexArray : public SuperExpression
{
public:
	std::string arrayName;
	std::shared_ptr<SuperExpression> index;

	IndexArray() = default;
	IndexArray(std::string& arrayName, std::shared_ptr<SuperExpression> index) 
		: arrayName(arrayName), index(index)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "IndexArray(array: " << arrayName << ", Index: " << index->printToString() << ")";
		return ss.str();
	}
	

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::IndexArray;
	}
};
