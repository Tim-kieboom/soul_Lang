#pragma once
#include "SuperStatement.h"
#include "Variable.h"

class RawArrayToSoulArray : public SuperStatement
{
public:
	std::shared_ptr<Variable> rawArray;
	std::shared_ptr<Variable> arraySize;
	std::shared_ptr<Variable> soulArray;
	std::string type;

	RawArrayToSoulArray() = default;
	RawArrayToSoulArray(std::shared_ptr<Variable> rawArray, std::shared_ptr<Variable> arraySize, std::shared_ptr<Variable> soulArray, std::string type)
		: rawArray(rawArray), arraySize(arraySize), soulArray(soulArray), type(type)
	{
	}

	void print() const override
	{
		std::cout << printToString();
	}

	std::string printToString() const override
	{
		std::stringstream ss;
		ss << "RawArrayToSoulArray(rawAarray: " << rawArray->printToString() << ", arraySize: " << arraySize->printToString() << ", soulArray: "<< soulArray->printToString() <<", Type(" << type << "))";
		return ss.str();
	}

	SyntaxNodeId getId() const override
	{
		return SyntaxNodeId::RawArrayToSoulArray;
	}
};
