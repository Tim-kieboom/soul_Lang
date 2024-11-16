#pragma once
#include <memory>
#include <cstdint>
#include <stdexcept>

#include "stringTools.h"

struct ErrorInfo
{
	const char* message = nullptr;
	uint64_t lineNumber = 0;

	ErrorInfo() = default;
	ErrorInfo(const char* message, uint64_t lineNumber)
		: message(message), lineNumber(lineNumber)
	{
	}

	ErrorInfo(const std::string& message, uint64_t lineNumber)
		: message(string_copyTo_c_str(message)), lineNumber(lineNumber)
	{
	}
};

template <typename T>
struct Result
{
private:
	T value_;

public:
	bool hasError = false;
	ErrorInfo error;

	Result() = default;

	//to transfer error between diffrentTypes Result
	template <typename V>
	Result(const Result<V>& other)
		: hasError(other.hasError), value_(), error(other.error)
	{
	}

	Result(T value)
		: hasError(false), value_(value), error()
	{
	}

	Result(ErrorInfo error)
		: hasError(true), value_(), error(error)
	{
	}

	T& value()
	{
		//if (hasError) 
		//	throw std::runtime_error("Attempting to access value of an error Result");

		return value_;
	}
};