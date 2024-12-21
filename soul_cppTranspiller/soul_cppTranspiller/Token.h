#pragma once
#include <string>

struct Token
{
    std::string text = "";
    uint64_t lineNumber = 0;

    Token() = default;
    Token(std::string text, uint64_t lineNumber)
        : text(text), lineNumber(lineNumber)
    {
    }
};