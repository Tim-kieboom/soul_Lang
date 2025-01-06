#pragma once
#include <string>
#include <exception>

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

template<typename V, typename E = ErrorInfo>
class Result
{
private:
	V value_;

public:
	E error;
	bool hasError;

	Result()
		: value_(), hasError(false), error()
	{
	}

	Result(V value)
		: value_(std::move(value)), hasError(false), error()
	{
	}

	Result(E error)
		: value_(), hasError(true), error(std::move(error))
	{
	}

	V& value();
};

// Specialization for void
template <typename E>
struct Result<void, E>
{
public:
	bool hasError = false;
	E error;

	Result()
		: hasError(false), error()
	{
	}

	Result(E error)
		: hasError(true), error(error)
	{
	}

	void value() {}
};

template<typename V, typename E>
inline V& Result<V, E>::value()
{
	if (hasError)
		throw std::exception("Attempting to access value of an error Result");

	return value_;
}