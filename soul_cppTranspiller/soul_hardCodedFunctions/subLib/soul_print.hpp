#pragma once
#include "soul_str.hpp"

template<typename T, typename = void>
inline void print(T msg)
{
    std::cout << msg;
}

template<>
inline void print(__Soul_STR__ msg)
{
    for (uint32_t i = 0; i < msg.size - msg.offset; i++)
        std::cout << msg.__soul_UNSAFE_at__(i);
}

template<typename K>
inline void print(__Soul_ARRAY__<K> array)
{
    std::cout << '[';

    uint32_t lastIndex = array.size - array.offset - 1;
    for (uint32_t i = 0; i < lastIndex; i++)
    {
        print_element(array.__soul_UNSAFE_at__(i), std::is_same<K, __Soul_STR__>());
        std::cout << ", ";
    }

    print_element(array.__soul_UNSAFE_at__(lastIndex), std::is_same<K, __Soul_STR__>());
    std::cout << ']';
}
template<typename T>
inline void print_element(const T& element, std::false_type)
{
    print(element);
}

template<typename T>
inline void print_element(const T& element, std::true_type)
{
    std::cout << '"';
    print(element);
    std::cout << '"';
}

template<> inline void print<int8_t>(int8_t msg) { std::cout << static_cast<int16_t>(msg); }
template<> inline void print<uint8_t>(uint8_t msg) { std::cout << static_cast<uint16_t>(msg); }

template<typename T, typename = void>
inline void println(T msg)
{
    std::cout << msg << std::endl;
}

template<>
inline void println(__Soul_STR__ msg)
{
    for (uint32_t i = 0; i < msg.size - msg.offset; i++)
        std::cout << msg.__soul_UNSAFE_at__(i);

    std::cout << std::endl;
}

template<typename K>
inline void println(__Soul_ARRAY__<K> array)
{
    print(array);
    std::cout << std::endl;
}

template<> inline void println<int8_t>(int8_t msg) { std::cout << static_cast<int16_t>(msg) << std::endl; }
template<> inline void println<uint8_t>(uint8_t msg) { std::cout << static_cast<uint16_t>(msg) << std::endl; }

