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

	Result()
		: value_(), hasError(false), error()
	{
	}

	Result(T value)
		: value_(value), hasError(false), error()
	{
	}

	Result(ErrorInfo error)
		: value_(), hasError(true), error(error)
	{
	}

	T& value();
};

template<typename T>
inline T& Result<T>::value()
{
	if (hasError)
		throw std::runtime_error("Attempting to access value of an error Result");

	return value_;
}
