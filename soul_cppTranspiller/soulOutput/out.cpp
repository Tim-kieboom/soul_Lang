#include <stdexcept>
#include <utility>
#include <cstdint>
#include <sstream>
#include <assert.h>
#include <iostream>
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
    if (!condition)
        throw std::runtime_error(msg);
#endif
}
 
template<typename T>
void print(T msg)
{
    std::cout << msg;
}
 
template<typename T>
void println(T msg)
{
    std::cout << msg << std::endl;
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
const char* __soul_format_string__(Args&&... args)
{
    std::ostringstream oss;
    __soul__append_to_stream__(oss, std::forward<Args>(args)...);
    std::string str = oss.str();
    char* buffer = new char[str.size() + 1];
    copy(str.begin(), str.end(), buffer);
    buffer[str.size()] = '\0';
    return (const char*)buffer;
}
 

constexpr const char* __0c_str__ = "if is true";
constexpr const char* __1c_str__ = "true";
constexpr const char* __2c_str__ = "false";
constexpr const char* __3c_str__ = "testNesting: ";
constexpr const char* __4c_str__ = "";
constexpr const char* __5c_str__ = "do not print";
constexpr const char* __6c_str__ = "print else if";
constexpr const char* __7c_str__ = "print else";
constexpr const char* __8c_str__ = " ()--,./\\;' \t \n \r \\ \' {} !@#$%^&*() \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\" ";
constexpr const char* __9c_str__ = "------ sum ------\n";
constexpr const char* __10c_str__ = "\n\n------ sumPlusOne ------\n";
constexpr const char* __11c_str__ = "sumPlus: ";
constexpr const char* __12c_str__ = "\n\n------ increment ------\n";
constexpr const char* __13c_str__ = "first value: ";
constexpr const char* __14c_str__ = "after increment value: ";
constexpr const char* __15c_str__ = ", ";

int32_t sum (/*default*/ const int32_t& a, /*default*/ const int32_t& b)
{
	return a+b;
}
int32_t sumPlusOne (/*default*/ const int32_t& a, /*mut*/ int32_t b)
{
	b++;
	return a+b;
}
void increment (/*out*/ int32_t& a)
{
	a++;
}
void testIf (/*default*/ const bool& condition)
{
	if(condition){
	println(__0c_str__);
	int32_t testNesting = 1;
	const char* boolString;	if(testNesting){
	boolString = __1c_str__;
}
	else{
	boolString = __2c_str__;
}
	println(__soul_format_string__(__3c_str__, boolString, __4c_str__));
}
	if(condition==true){
	println(__0c_str__);
}
	if(!condition){
	println(__5c_str__);
}
	else{
	println(__0c_str__);
}
	if(!condition){
	println(__5c_str__);
}
	else if(condition){
	println(__6c_str__);
}
	if(!condition){
	println(__5c_str__);
}
	else if(!condition){
	println(__5c_str__);
}
	else{
	println(__7c_str__);
}
}
int32_t main ()
{
	const char* tokenizeHell = __8c_str__;
	testIf(true);
	println(__9c_str__);
	int32_t result = sum(1, 2);
	println(result);
	println(__10c_str__);
	result = sumPlusOne(1, 2);
	println(__soul_format_string__(__11c_str__, result, __4c_str__));
	println(__12c_str__);
	int32_t value = 1;
	println(__soul_format_string__(__13c_str__, value, __4c_str__));
	increment(/*out*/value);
	println(__soul_format_string__(__14c_str__, value, __4c_str__));
	int32_t i = 0;
	for(;i<10;
i++)
{
	print(__soul_format_string__(__4c_str__, i, __15c_str__));
}
	println(__4c_str__);
}
