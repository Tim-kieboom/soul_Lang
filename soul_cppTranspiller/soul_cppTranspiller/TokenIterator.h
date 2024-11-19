#pragma once
#include <cstdint>
#include "tokenizer.h"

struct TokenIterator
{
	int64_t i = -1;
	int64_t currentLine = -1;
	const std::vector<Token>& tokens;

	TokenIterator(const std::vector<Token>& tokens)
		: tokens(tokens)
	{
	}

	bool peekToken(std::string& tokenText, int64_t step = 1)
	{
		if (i + step > (int64_t)tokens.size())
			return false;

		tokenText = tokens.at(i + step).text;
		return true;
	}

	bool nextToken(std::string& tokenText, int64_t step = 1)
	{
		if (i + step >= (int64_t)tokens.size())
			return false;

		i += step;
		const Token& token = tokens.at(i);
		tokenText = token.text;
		currentLine = token.lineNumber;
		return true;
	}

	bool skipToken(int64_t step = 1)
	{
		if (i + step > (int64_t)tokens.size())
			return false;

		i += step;
		const Token& token = tokens.at(i);
		currentLine = token.lineNumber;
		return true;
	}

	uint64_t size()
	{
		return tokens.size();
	}
};