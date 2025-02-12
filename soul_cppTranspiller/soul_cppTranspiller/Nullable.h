#pragma once
#include <exception>

template <typename T>
class Nullable
{
private:
	bool isNull_;
	T value_;

public:

	static Nullable<T> makeEmpty() 
	{
		return Nullable<T>();
	}

	Nullable()
		: isNull_(true), value_({})
	{
	}

	Nullable(T value)
		: isNull_(false), value_(value)
	{
	}

	bool isNull() const
	{
		return isNull_;
	}

	bool hasValue() const 
	{
		return !isNull_;
	}

	bool tryValue(T& _value)
	{
		if (isNull_)
			return false;

		_value = value_;
		return true;
	}

	const T& value() const
	{
		if (isNull_)
			throw std::exception("trying to get value while value is null");

		return value_;
	}

	T& value()
	{
		if (isNull_)
			throw std::exception("trying to get value while value is null");

		return value_;
	}
};