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
const char* __soul_format__(Args&&... args)
{
    std::ostringstream oss;
    __soul__append_to_stream__(oss, std::forward<Args>(args)...);

    string str = oss.str();
    char* buffer = new char[str.size() + 1];
    copy(str.begin(), str.end(), buffer);
    buffer[str.size()] = '\0';

    return (const char*)buffer;
}

template <typename T>
class soul_optional 
{
private:
	bool has_value_;
	alignas(T) unsigned char storage_[sizeof(T)];

    constexpr void clearStorage()
    {
        if (has_value_)
        {
            reinterpret_cast<T*>(storage_)->~T();
            has_value_ = false;
        }
    }

public:
    ~soul_optional()
    {
        clearStorage();
    }

    // Constructor empty
    constexpr soul_optional() noexcept : has_value_(false) {}

    // Constructor from value
    constexpr soul_optional(const T& value) : has_value_(true)
    {
        new (storage_) T(value);
    }

    // Move constructor
    constexpr soul_optional(T&& value) : has_value_(true)
    {
        new (storage_) T(std::move(value));
    }

    // Copy constructor
    constexpr soul_optional(const soul_optional& other) : has_value_(other.has_value_)
    {
        if (has_value_) 
        {
            new (storage_) T(*other);
        }
    }

    // Assignment operators
    constexpr soul_optional& operator=(const soul_optional& other)
    {
        if (this != &other) 
        {
            clearStorage();
            has_value_ = other.has_value_;
            if (has_value_) 
            {
                new (storage_) T(*other);
            }
        }
        return *this;
    }

    constexpr soul_optional& operator=(soul_optional&& other) noexcept
    {
        if (this != &other) 
        {
            clearStorage();
            has_value_ = other.has_value_;
            if (has_value_) {
                new (storage_) T(std::move(*other));
                other.clearStorage();
            }
        }
        return *this;
    }

    constexpr bool has_value() const noexcept 
    {
        return has_value_;
    }

    // Access the contained value
    constexpr T& value()&
    {
        if (!has_value_) 
            throw std::runtime_error("Optional object has no value");

        return *reinterpret_cast<T*>(storage_);
    }

    constexpr const T& value() const&
    {
        if (!has_value_)
            throw std::runtime_error("Optional object has no value");

        return *reinterpret_cast<const T*>(storage_);
    }

    constexpr T&& value()&&
    {
        if (!has_value_)
            throw std::runtime_error("Optional object has no value");

        return std::move(*reinterpret_cast<T*>(storage_));
    }

    // Access the contained value or return a default
    constexpr T value_or(T&& default_value) const&
    {
        return has_value_ ? **this : static_cast<T>(std::forward<T>(default_value));
    }

    constexpr T value_or(T&& default_value)&&
    {
        return has_value_ ? std::move(**this) : static_cast<T>(std::forward<T>(default_value));
    }

    constexpr T* operator->() 
    {
        return &value();
    }

    constexpr const T* operator->() const
    {
        return &value();
    }

    constexpr T& operator*()&
    {
        return value();
    }

    constexpr const T& operator*() const&
    {
        return value();
    }

    constexpr T&& operator*()&&
    {
        return std::move(value());
    }

    // Boolean conversion
    constexpr explicit operator bool() const noexcept
    {
        return has_value_;
    }
};

#define assertFail(condition, msg) assert((msg, condition))

static inline void assertThrow(bool condition, const char* msg)
{
#ifndef NDEBUG
	if(!condition)
    	throw std::runtime_error(msg);
#endif
}
