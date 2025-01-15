#pragma once
#include "ISyntaxNode.h"

// is a base class for all the StatmentNodes
class SuperStatement : public ISyntaxNode {};

template <typename T>
struct BodyStatment_Result
{
	std::vector<std::shared_ptr<SuperStatement>> beforeStatment;
	std::shared_ptr<T> expression;
	std::vector<std::shared_ptr<SuperStatement>> afterStatment;

	BodyStatment_Result() = default;

	BodyStatment_Result(std::shared_ptr<T> expression)
		: expression(expression)
	{
	}

	template <typename K>
	void addToBodyResult(BodyStatment_Result<K>& other)
	{
		for (auto& stament : other.beforeStatment)
			beforeStatment.push_back(stament);

		for (auto& stament : other.afterStatment)
			afterStatment.push_back(stament);
	}
};
