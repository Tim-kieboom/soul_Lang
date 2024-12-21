#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace _privEqualTools
{
	template<typename T, typename UINT>
	static inline bool _uintArrayEquals(const T& _one, const T& _two, uint64_t size)
	{
		const UINT* one = (UINT*)&_one;
		const UINT* two = (UINT*)&_two;
		for (uint64_t i = 0; i < size; i++)
		{
			if (one[i] != two[i])
				return false;
		}

		return true;
	}
}

template<typename T>
inline bool structEquals(const T& one, const T& two)
{
	uint64_t typeSize = sizeof(T);
	
	if (typeSize % 8 == 0)
		return _privEqualTools::_uintArrayEquals<T, uint64_t>(one, two, typeSize / 8);

	if(typeSize % 4 == 0)
		return _privEqualTools::_uintArrayEquals<T, uint32_t>(one, two, typeSize / 4);

	if(typeSize % 2 == 0)
		return _privEqualTools::_uintArrayEquals<T, uint16_t>(one, two, typeSize / 2);

	return _privEqualTools::_uintArrayEquals<T, uint8_t>(one, two, typeSize);
}

template<typename T>
inline bool vectorEquals(const std::vector<T>& one, const std::vector<T>& two)
{
	if (one.size() != two.size())
		return false;

	uint64_t typeSize = sizeof(T);
	if (typeSize % 8 == 0)
		return _privEqualTools::_uintArrayEquals<const T*, uint64_t>(one.data(), two.data(), typeSize / 8);

	if (typeSize % 4 == 0)
		return _privEqualTools::_uintArrayEquals<const T*, uint32_t>(one.data(), two.data(), typeSize / 4);

	if (typeSize % 2 == 0)
		return _privEqualTools::_uintArrayEquals<const T*, uint16_t>(one.data(), two.data(), typeSize / 2);

	return _privEqualTools::_uintArrayEquals<const T*, uint8_t>(one.data(), two.data(), typeSize);
}
	