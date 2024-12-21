#pragma once
#include <memory>
#include <string>
#include <cstdint>
#include <stdexcept>

#include "stringTools.h"

struct ErrorInfo
{
	std::string message;
	uint64_t lineNumber = 0;

	ErrorInfo() = default;
	ErrorInfo(const char* message, uint64_t lineNumber)
		: message(std::string(message)), lineNumber(lineNumber)
	{
	}

	ErrorInfo(const std::string& message, uint64_t lineNumber)
		: message(message), lineNumber(lineNumber)
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

// Specialization for void
template <>
struct Result<void>
{
public:
	bool hasError = false;
	ErrorInfo error;

	Result()
		: hasError(false), error()
	{
	}

	Result(ErrorInfo error)
		: hasError(true), error(error)
	{
	}

	void value() {} // No-op function for void specialization
};

template<typename T>
inline T& Result<T>::value()
{
	if (hasError)
		throw std::runtime_error("Attempting to access value of an error Result");

	return value_;
}
