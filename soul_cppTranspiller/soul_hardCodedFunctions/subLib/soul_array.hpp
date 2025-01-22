#pragma once
#include "soul_cppLibs.h"

template<typename T>
class __Soul_ARRAY_Iterator__
{
private:
    T* ptr;

public:
    typedef std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;

    __Soul_ARRAY_Iterator__(T* ptr) 
        : ptr(ptr)
    {
    }

    T& operator*() { return *ptr; }
    __Soul_ARRAY_Iterator__& operator++() { ++ptr; return *this; }
    __Soul_ARRAY_Iterator__ operator++(int) { __Soul_ARRAY_Iterator__ tmp = *this; ++ptr; return tmp; }
    bool operator!=(const __Soul_ARRAY_Iterator__& other) const { return ptr != other.ptr; }
    bool operator==(const __Soul_ARRAY_Iterator__& other) const { return ptr == other.ptr; }
};

template<typename T>
class __Soul_ARRAY_ConstIterator__
{
private:
    const T* ptr;

public:
    typedef std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;

    __Soul_ARRAY_ConstIterator__(const T* ptr)
        : ptr(ptr)
    {
    }

    const T& operator*() const { return *ptr; }
    const T* operator->() const { return ptr; }
    __Soul_ARRAY_ConstIterator__& operator++() { ++ptr; return *this; }
    __Soul_ARRAY_ConstIterator__ operator++(int) { __Soul_ARRAY_ConstIterator__ tmp = *this; ++ptr; return tmp; }
    bool operator!=(const __Soul_ARRAY_ConstIterator__& other) const { return ptr != other.ptr; }
    bool operator==(const __Soul_ARRAY_ConstIterator__& other) const { return ptr == other.ptr; }
};

template <typename T>
class __Soul_ARRAY__
{
public:
    std::vector<T> rawArray;
    uint32_t offset;
    uint32_t size;

    __Soul_ARRAY__() = default;
    ~__Soul_ARRAY__() = default;

    __Soul_ARRAY__(std::initializer_list<T> initList)
        : rawArray(initList.size()), offset(0), size(initList.size())
    {
        uint32_t i = 0;
        for (const T& el : initList)
            ((T*)rawArray.get())[i++] = el;
    }

    __Soul_ARRAY__(uint32_t size)
        : rawArray(size), offset(0), size(size)
    {
    }

    __Soul_ARRAY__(__Soul_ARRAY__<T>* other, uint32_t start, uint32_t end)
        : rawArray(other->rawArray), offset(start), size(end)
    {
    }

    // __Soul_ARRAY__(const __Soul_ARRAY__& other)
    //     : rawArray(new T[other.size], std::default_delete<T[]>()), offset(0), size(other.size)
    // {
    //     std::copy(other.begin(), other.end(), rawArray.get());
    // }


    constexpr T& operator[](int64_t index) noexcept
    {
        if (index < 0)
            index = index + size;

        index += offset;

        if (index >= size || index < 0)
        {
            printf("!!ERROR!! index out of range\n");
            exit(1);
        }

        return *(rawArray.data() + (uint32_t)index);
    }

    constexpr T& __soul_UNSAFE_at__(uint32_t index) noexcept
    {
        return *(rawArray.data() + index + offset);
    }

    constexpr __Soul_ARRAY__<T> __soul_makeSpan_fail__(int64_t start, int64_t end) noexcept
    {
        if (start < 0)
            start = start + size;

        if (end < 0)
            end = end + size;

        start += offset;
        end += offset + 1;

        if (end < start)
        {
            printf("!!ERROR!! (start < end) while making arraySpan\n");
            exit(1);
        }

        return __Soul_ARRAY__<T>(this, start, end);
    }

    __Soul_ARRAY_Iterator__<T> begin() 
    {
        return __Soul_ARRAY_Iterator__<T>(((T*)rawArray.data()) + offset);
    }

    __Soul_ARRAY_Iterator__<T> end()
    {
        return __Soul_ARRAY_Iterator__<T>(((T*)rawArray.data()) + size);
    }

    __Soul_ARRAY_ConstIterator__<T> begin() const
    {
        return __Soul_ARRAY_ConstIterator__<T>(((T*)rawArray.data() + offset));
    }

    __Soul_ARRAY_ConstIterator__<T> end() const
    {
        return __Soul_ARRAY_ConstIterator__<T>(((T*)rawArray.data()) + size);
    }
};

template <typename T, typename = void>
inline T __Soul_copy__(T other) { return other; }

template <typename K>
inline __Soul_ARRAY__<K> __Soul_copy__(__Soul_ARRAY__<K> other)
{
    __Soul_ARRAY__<K> copyArray(other.size);
    uint32_t i = 0;
    for(const K& el : other)
        copyArray.__soul_UNSAFE_at__(i++) = __Soul_copy__(el);

    return copyArray;
}
  