#pragma once
#include "../soul_cppLibs.h"

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
void __Soul_ARRAY_deleter__(T* ptr) 
{
    delete[] ptr;
}

template <typename T>
class __Soul_ARRAY__
{
private:
    std::shared_ptr<T> rawArray;
    uint32_t offset_ = 0;
    uint32_t size_ = 0;

public:

    __Soul_ARRAY__() = default;
    ~__Soul_ARRAY__() = default;

    __Soul_ARRAY__(std::initializer_list<T> initList)
        : offset_(0), size_(initList.size())
    {
        rawArray = std::shared_ptr<T>(new T[size_], __Soul_ARRAY_deleter__<T>);

        uint32_t i = 0;
        for (const T& el : initList)
            ((T*)rawArray.get())[i++] = el;
    }

    __Soul_ARRAY__(uint32_t size_)
        : offset_(0), size_(size_)
    {
        if(size_ != 0)
            rawArray = std::shared_ptr<T>(new T[size_], __Soul_ARRAY_deleter__<T>);
    }

    __Soul_ARRAY__(__Soul_ARRAY__<T>* other, uint32_t start, uint32_t end)
        : rawArray(other->rawArray), offset_(start), size_(end)
    {
    }

    constexpr T& operator[](int64_t index) noexcept
    {
        if (index < 0)
            index = index + size_;

        index += offset_;

        if (index >= size_ || index < 0)
        {
            printf("!!ERROR!! index out of range\n");
            exit(1);
        }

        return *( ((T*)rawArray.get()) + (uint32_t)index );
    }

    __Soul_ARRAY__ operator+(__Soul_ARRAY__<T>& other)
    {
        int64_t thisSize = (this->size_ - this->offset_);
        int64_t otherSize = (other.size_ - other.offset_);
        int64_t newSize = thisSize + otherSize;
        if(newSize <= 0)
            return __Soul_ARRAY__();
        
        __Soul_ARRAY__ newStr(newSize);
        for(int64_t i = 0; i < thisSize; i++)
            newStr.__soul_UNSAFE_at__(i) = this->__soul_UNSAFE_at__(i);

        for(int64_t i = 0; i < otherSize; i++)
            newStr.__soul_UNSAFE_at__(i + thisSize) = other.__soul_UNSAFE_at__(i);

        return newStr;
    }

    bool operator==(__Soul_ARRAY__<T>& other)
    {
        if((int64_t)(this->size_) - this->offset_ != (int64_t)(other.size_) - other.offset_)
            return false;

        for(uint32_t i = 0; i < this->size_; i++)
        {    
            if(this->__soul_UNSAFE_at__(i) != other.__soul_UNSAFE_at__(i))
                return false;
        }

        return true;
    }

    constexpr T& __soul_UNSAFE_at__(uint32_t index) noexcept
    {
        return *( ((T*)rawArray.get()) + index + offset_ );
    }

    constexpr __Soul_ARRAY__<T> __soul_makeSpan_fail_start_end__(int64_t start, int64_t end) noexcept
    {
        if (start < 0)
            start = start + size_;

        if (end < 0)
            end = end + size_;

        start += offset_;
        end += offset_ + 1;

        if (end < start)
        {
            printf("!!ERROR!! (start < end) while making arraySpan\n");
            exit(1);
        }

        return __Soul_ARRAY__<T>(this, start, end);
    }

    constexpr __Soul_ARRAY__<T> __soul_makeSpan_fail_start__(int64_t start) noexcept
    {
        if (start < 0)
            start = start + size_;

        start += offset_;

        if (size_ < start)
        {
            printf("!!ERROR!! (start < end) while making arraySpan\n");
            exit(1);
        }

        return __Soul_ARRAY__<T>(this, start, size_);
    }

    constexpr __Soul_ARRAY__<T> __soul_makeSpan_fail_end__(int64_t end) noexcept
    {
        if (end < 0)
            end = end + size_;

        end += offset_ + 1;

        if (end < offset_)
        {
            printf("!!ERROR!! (start < end) while making arraySpan\n");
            exit(1);
        }

        return __Soul_ARRAY__<T>(this, offset_, end);
    }

    uint32_t size() const
    {
        return size_;
    }

    uint32_t offset() const
    {
        return offset_;
    }

    __Soul_ARRAY_Iterator__<T> begin() 
    {
        return __Soul_ARRAY_Iterator__<T>(rawArray.get() + offset_);
    }

    __Soul_ARRAY_Iterator__<T> end()
    {
        return __Soul_ARRAY_Iterator__<T>(rawArray.get() + size_);
    }

    __Soul_ARRAY_ConstIterator__<T> begin() const
    {
        return __Soul_ARRAY_ConstIterator__<T>(rawArray.get() + offset_);
    }

    __Soul_ARRAY_ConstIterator__<T> end() const
    {
        return __Soul_ARRAY_ConstIterator__<T>(rawArray.get() + size_);
    }
};

template <typename T>
inline uint64_t arrSize(const __Soul_ARRAY__<T>& arr) 
{
    return arr.size(); 
}

template <typename T, typename = void>
inline T __Soul_copy__(T other) { return other; }

template <typename K>
inline __Soul_ARRAY__<K> __Soul_copy__(const __Soul_ARRAY__<K>& other)
{
    __Soul_ARRAY__<K> copyArray(other.size());
    uint32_t i = 0;
    for(const K& el : other)
        copyArray.__soul_UNSAFE_at__(i++) = __Soul_copy__(el);

    return copyArray;
}
  