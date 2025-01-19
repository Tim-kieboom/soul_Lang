#pragma once
#include <exception>

template <typename T>
class Nullable
{
public:
	bool isNull;
	T value;

	Nullable()
		: isNull(true), value()
	{
	}

	Nullable(T value)
		: isNull(false), value(value)
	{
	}

	bool tryGetValue(T& _value)
	{
		if (isNull)
			return false;

		_value = value;
		return true;
	}

	T& getValue()
	{
		if (isNull)
			throw std::exception("trying to get value while value is null");

		return value;
	}
};