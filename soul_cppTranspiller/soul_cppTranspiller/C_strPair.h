#pragma once
#include <string>

struct C_strPair
{
    std::string name;
    std::string value;

    C_strPair() = default;

    C_strPair(std::string& name, std::string& value)
        : name(name), value(value)
    {
    }

    C_strPair(const char* name, const char* value)
        : name(std::string(name)), value(std::string(value))
    {
    }
};