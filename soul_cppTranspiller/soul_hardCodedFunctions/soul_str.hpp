#pragma once 
#include "soul_array.hpp"

class __Soul_STR__
{
public:
    std::string rawStr;
    uint32_t offset = 0;
    uint32_t size = 0;

    __Soul_STR__() = default;
    ~__Soul_STR__() = default;

    __Soul_STR__(const char* rawStr)
        : rawStr(rawStr), offset(0), size(size)
    {
        this->size = this->rawStr.size();
    }

    __Soul_STR__(std::string& rawStr)
        : rawStr(rawStr), offset(0), size(rawStr.size())
    {
    }

    __Soul_STR__(__Soul_STR__* other, uint32_t start, uint32_t end)
        : rawStr(other->rawStr), offset(start), size(end)
    {
    }

    char& operator[](int64_t index) noexcept
    {
        if (index < 0)
            index = index + size;

        index += offset;

        if (index >= size)
        {
            printf("!!ERROR!! index out of range\n");
            exit(1);
        }

        return rawStr[index];
    }

    __Soul_STR__ operator+(__Soul_STR__& other)
    {
        std::string newStr = this->rawStr + other.rawStr;
        return __Soul_STR__(newStr);
    }

    bool operator==(__Soul_STR__& other)
    {
        return this->rawStr == other.rawStr;
    }

    char& __soul_UNSAFE_at__(uint32_t index) noexcept
    {
        return rawStr[index + offset];
    }

    __Soul_STR__ __soul_makeSpan_fail__(int64_t start, int64_t end) noexcept
    {
        if (start < 0)
            start = start + size;

        if (end < 0)
            end = end + size;

        start += offset;
        end += offset + 1;

        if (end < start)
        {
            printf("!!ERROR!! (start < end) while making stringSpan\n");
            exit(1);
        }

        return __Soul_STR__(this, start, end);
    }

    __Soul_ARRAY_Iterator__<char> begin()
    {
        return __Soul_ARRAY_Iterator__<char>((char*)rawStr.c_str() + offset);
    }

    __Soul_ARRAY_Iterator__<char> end()
    {
        return __Soul_ARRAY_Iterator__<char>((char*)rawStr.c_str() + size);
    }

    __Soul_ARRAY_ConstIterator__<char> begin() const
    {
        return __Soul_ARRAY_ConstIterator__<char>(rawStr.c_str() + offset);
    }

    __Soul_ARRAY_ConstIterator__<char> end() const
    {
        return __Soul_ARRAY_ConstIterator__<char>(rawStr.c_str() + size);
    }
};

inline __Soul_STR__ __Soul_copy__(__Soul_STR__ other)
{
    std::string copyStr = other.rawStr;
    return __Soul_STR__(copyStr);
}

template <typename T>
void __soul__append_to_stream__(std::ostringstream& oss, T&& arg)
{
    oss << std::forward<T>(arg);
}

template <typename T, typename... Args>
void __soul__append_to_stream__(std::ostringstream& oss, T&& arg, Args&&... args)
{
    oss << std::forward<T>(arg);
    __soul__append_to_stream__(oss, std::forward<Args>(args)...);
}

template <typename ...Args>
inline __Soul_STR__ __soul_format_string__(Args&&... args)
{
    std::ostringstream oss;
    __soul__append_to_stream__(oss, std::forward<Args>(args)...);
    return toStr(oss.str());
}

inline __Soul_STR__ toStr(const char* cppStr) 
{
    return __Soul_STR__(cppStr);
}

inline __Soul_STR__ toStr(std::string cppStr) 
{
    return __Soul_STR__(cppStr);
}

inline __Soul_STR__ toStr(const bool value)
{
    const char* strValue = value ? "true" : "false"; 
    return toStr(strValue);
}

inline __Soul_STR__ toStr(const char value)     { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const float value)    { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const double value)   { return toStr(std::to_string(value)); } 
inline __Soul_STR__ toStr(const int16_t value)  { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const int32_t value)  { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const int64_t value)  { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const uint32_t value) { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const uint16_t value) { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const uint64_t value) { return toStr(std::to_string(value)); }
inline __Soul_STR__ toStr(const int8_t value)   { return toStr(std::to_string((int16_t)value)); }
inline __Soul_STR__ toStr(const uint8_t value)  { return toStr(std::to_string((uint16_t)value)); }
