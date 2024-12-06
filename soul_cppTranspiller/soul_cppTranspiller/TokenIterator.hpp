#pragma once
#include <string>
#include <vector>

#include "Token.h"

struct TokenIterator
{
	std::string currentToken;

	int64_t i = -1;
	int64_t currentLine = -1;
	const std::vector<Token>& tokens;
	
	TokenIterator(const std::vector<Token>& tokens)
		: tokens(tokens)
	{
	}

	bool peekToken(std::string& token, int64_t step = 1)
	{
		if (i + step > (int64_t)tokens.size())
			return false;

		token = tokens.at(i + step).text;
		return true;
	}

	bool nextToken(int64_t step = 1)
	{
		if (i + step >= (int64_t)tokens.size())
			return false;

		i += step;
		const Token& token = tokens.at(i);
		currentToken = token.text;
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