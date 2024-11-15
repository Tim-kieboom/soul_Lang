#pragma once
#include <string>

typedef uint64_t ui64_string;

ui64_string string_hash(const char* str);
ui64_string string_hash(const std::string& str);

constexpr ui64_string _string_consexprHash(const char* str, uint64_t i, uint64_t stringlen, char ch, int32_t p, int32_t m, uint64_t hash, uint64_t p_pow) noexcept
{
	return (i + 1 >= stringlen) ? hash
		: _string_consexprHash
		(
			str,
			i + 1,
			stringlen,
			str[i + 1],
			p,
			m,
			(hash + (ch - 'a' + 1) * p_pow % m),
			((p_pow * p) % m)
		);
}

constexpr ui64_string string_consexprHash(const char* str) noexcept
{
	return _string_consexprHash(str, 0, strlen(str), str[0], 31, (int32_t)(1'000'000'009), 0, 1);
}

constexpr ui64_string string_consexprHash(const char* str, int32_t magicNumber) noexcept
{
	return _string_consexprHash(str, 0, strlen(str), str[0], 31, magicNumber, 0, 1);
}