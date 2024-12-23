#include <stdexcept>
#include <utility>
#include <cstdint>
#include <sstream>
#include <assert.h>
#include <iostream>
#define null nullptr
 
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
 
static inline void assertFail(bool condition, const char* msg)
{
#ifndef NDEBUG
    if (!condition)
    {  
        std::cout << msg << std::endl;
        assert(("assertFail failed (read last console message)", false));
    }
#endif
}
 
static inline void assertThrow(bool condition, const char* msg)
{
#ifndef NDEBUG
    if (!condition)
        throw std::runtime_error(msg);
#endif
}
 
template<typename T, typename = void>
void print(T msg)
{
    std::cout << msg;
}
 
template<>
void print<int8_t>(int8_t msg)
{
    std::cout << static_cast<int16_t>(msg);
}
 
template<>
void print<uint8_t>(uint8_t msg)
{
    std::cout << static_cast<uint16_t>(msg);
}
 
 
template<typename T, typename = void>
void println(T msg)
{
    std::cout << msg << std::endl;
}
 
template<>
void println<int8_t>(int8_t msg)
{
    std::cout << static_cast<int16_t>(msg) << std::endl;
}
 
template<>
void println<uint8_t>(uint8_t msg)
{
    std::cout << static_cast<uint16_t>(msg) << std::endl;
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
std::string __soul_format_string__(Args&&... args)
{
    std::ostringstream oss;
    __soul__append_to_stream__(oss, std::forward<Args>(args)...);
    return oss.str();
}
 

constexpr const char* __20c_str__ = " shouldBe=1";
constexpr const char* __0c_str__ = "if is true";
constexpr const char* __1c_str__ = "true";
constexpr const char* __2c_str__ = "false";
constexpr const char* __3c_str__ = "testNesting: ";
constexpr const char* __4c_str__ = "";
constexpr const char* __5c_str__ = "do not print";
constexpr const char* __6c_str__ = "print else if";
constexpr const char* __7c_str__ = "print else";
constexpr const char* __8c_str__ = "\n------ forLoop ------\n";
constexpr const char* __9c_str__ = ", ";
constexpr const char* __10c_str__ = " ()--,./\\;' \t \n \r \\ \' {} !@#$%^&*() \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\" ";
constexpr const char* __11c_str__ = "------ sum ------\n";
constexpr const char* __12c_str__ = "\n\n------ sumPlusOne ------\n";
constexpr const char* __13c_str__ = "sumPlus: ";
constexpr const char* __14c_str__ = "\n\n------ increment ------\n";
constexpr const char* __15c_str__ = "first value: ";
constexpr const char* __16c_str__ = "after increment value: ";
constexpr const char* __17c_str__ = "\n\n------ testArray ------\n";
constexpr const char* __18c_str__ = "array[0]=";
constexpr const char* __19c_str__ = " shouldBe=0, array[1]=";

int32_t sum(/*default*/const int32_t a, /*default*/const int32_t b)
{
	return a+b;
}

double sum(/*default*/const double a, /*default*/const double b)
{
	return a+b;
}

int8_t testArg(/*default*/const int8_t a, /*default*/const int8_t b)
{
	return a;
}

int32_t sumPlusOne(/*default*/const int32_t a, /*mut*/int32_t b)
{
	b++;
	return a+b;
}

void increment(/*out*/int32_t& a)
{
	a++;
}

void testIf(/*default*/const bool condition)
{
	if(condition)
	{
		println(__0c_str__);
		int32_t testNesting = 1;
		std::string boolString;
		if(testNesting)
		{
			boolString = __1c_str__;
		}

		else
		{
			boolString = __2c_str__;
		}

		println(__soul_format_string__(__3c_str__, boolString, __4c_str__));
	}

	if(condition==true)
	{
		println(__0c_str__);
	}

	if(!condition)
	{
		println(__5c_str__);
	}

	else
	{
		println(__0c_str__);
	}

	if(!condition)
	{
		println(__5c_str__);
	}

	else if(condition)
	{
		println(__6c_str__);
	}

	if(!condition)
	{
		println(__5c_str__);
	}

	else if(!condition)
	{
		println(__5c_str__);
	}

	else
	{
		println(__7c_str__);
	}

}

void testForLoop()
{
	println(__8c_str__);
	int32_t i = 0;
	for(;i<10;i++)
	{
		print(__soul_format_string__(__4c_str__, i, __9c_str__));
	}

	println(__4c_str__);
}

int32_t main()
{
	const std::string tokenizeHell = __10c_str__;
	int8_t b = 86;
	println(b);
	println(__11c_str__);
	int32_t result = sum(1, 2);
	double fl = 8;
	int32_t result2 = sum(fl, fl);
	println(result);
	println(result2);
	println(__12c_str__);
	result = sumPlusOne(1, 2);
	println(__soul_format_string__(__13c_str__, result, __4c_str__));
	println(__14c_str__);
	int32_t value = 1;
	println(__soul_format_string__(__15c_str__, value, __4c_str__));
	increment(/*out*/value);
	println(__soul_format_string__(__16c_str__, value, __4c_str__));
	testIf(true);
	testForLoop();
	int32_t* array = null;
	array = new int32_t[2];
	array [0] =0;
	array [1] =1;
	int32_t el = array[0];
	println(__17c_str__);
	println(__soul_format_string__(__18c_str__, array[0], __19c_str__, array[1], __20c_str__));
	delete array;
}

