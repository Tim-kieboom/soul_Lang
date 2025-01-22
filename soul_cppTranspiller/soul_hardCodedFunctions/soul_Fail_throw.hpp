#pragma once
#include "soul_cppLibs.h"

static inline void Throw(const std::string& msg)
{
    throw std::runtime_error(msg.c_str());
}

static inline void Fail(const std::string& msg)
{
    std::cout << msg.c_str() << std::endl;
    assert(("assertFail failed (read last console message)", false));
}

static inline void checkThrow(bool condition, const std::string& msg)
{
    if (condition)
        Throw(msg);
}

static inline void checkFail(bool condition, const std::string& msg)
{
    if (condition)
        Fail(msg);
}

static inline void checkFail_debug(bool condition, const std::string& msg)
{
#ifndef NDEBUG
    checkFail(condition, msg);
#endif
}

static inline void checkThrow_debug(bool condition, const std::string& msg)
{
#ifndef NDEBUG
    checkThrow(condition, msg);
#endif
}