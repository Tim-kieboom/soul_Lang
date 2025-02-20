#pragma once
#include "SuperExpression.h"

class IndexArray : public SuperExpression
{
public:
	std::shared_ptr<SuperExpression> array;
	std::shared_ptr<SuperExpression> index;

	IndexArray() = default;
	IndexArray(std::shared_ptr<SuperExpression> array, std::shared_ptr<SuperExpression> index)
		: array(array), index(index)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "IndexArray(array: " << array->printToString() << ", Index: " << index->printToString() << ")";
		return ss.str();
	}
	

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::IndexArray;
	}
};
