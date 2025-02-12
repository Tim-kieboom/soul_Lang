#include "soul_hardCodedFunctions.h"

class StructObject;
class List;
constexpr double sqrt(const double x);
constexpr const char* __0c_str__="";
constexpr const char* __1c_str__="can't increase capacity of List, because List reached max Capacity:";
constexpr const char* __2c_str__="can not peek empty list";
constexpr const char* __3c_str__="can not pop empty list";
constexpr const char* __4c_str__="ellie";
constexpr const char* __5c_str__="log:";
constexpr const char* __6c_str__=", pow:";
constexpr const char* __7c_str__=", root";
constexpr const uint32_t MAX_U32=__Soul_CompileConst_math__::pow(32, 2)-1;
constexpr double sqrt(const double x)
{
return root(x, 2);

}
class StructObject
{
public: __Soul_STR__ key=__0c_str__;
public: int32_t value=0;
public: StructObject()
{

}
public: StructObject(__Soul_STR__ key, int32_t value):key(key),value(value)
{

}
};
class List
{
private: __Soul_ARRAY__<int32_t> array=__Soul_ARRAY__<int32_t>(0);
public: size_t Size=0;
public: size_t Capacity=0;
public: List()
{

}
public: List(size_t Capacity):Capacity(Capacity)
{

}
public: List(__Soul_ARRAY__<int32_t> array, size_t Size):array(array),Size(Size)
{

}
public: void Push(const int32_t value)
{
constexpr const uint32_t LAST_CAPACITY_BEFORE_OVERFLOW=__Soul_CompileConst_math__::pow(16, 2);
if(Size+1>Capacity)
{
if(Capacity==0)
{
reserve(1);

}

else if(Capacity<LAST_CAPACITY_BEFORE_OVERFLOW)
{
reserve(Capacity*2);

}

else if(Capacity==MAX_U32)
{
Fail(__soul_format_string__(__1c_str__, MAX_U32, __0c_str__));

}

else
{
reserve(MAX_U32);

}


}

array[Size++]=value;

}
public: int32_t Peek()
{
checkFail(Size==0, __2c_str__);
return array[Size-1];

}
public: int32_t Pop()
{
checkFail(Size==0, __3c_str__);
return array[++Size];

}
private: void reserve(const size_t newCapacity)
{
array=__Soul_copy__(array, newCapacity);
Capacity=len(array);

}
};
int main(int __Soul_argc__, char** __Soul_argv__)
{
__Soul_ARRAY__<__Soul_STR__> args=__Soul_ARRAY__<__Soul_STR__>(__Soul_argc__);
for(uint64_t __CPP_i__ = 0; __CPP_i__ < __Soul_argc__; __CPP_i__++)
        args.__soul_UNSAFE_at__(__CPP_i__)=__Soul_argv__[__CPP_i__];
println();
__Soul_ARRAY__<__Soul_STR__> array;
array=__Soul_ARRAY__<__Soul_STR__>(10);
array[3]=__4c_str__;
println(len(array[3]));
println(array);
__Soul_ARRAY__<__Soul_STR__> array2=__Soul_copy__(array, 20);
double log_=__Soul_CompileConst_math__::root(__Soul_CompileConst_math__::pow(__Soul_CompileConst_math__::root(4, 2), __Soul_CompileConst_math__::log(100, 10)), 2);
constexpr const double POW=__Soul_CompileConst_math__::pow(4, 0.5);
constexpr const double ROOT=__Soul_CompileConst_math__::root(4, 2);
println(__soul_format_string__(__5c_str__, log_, __6c_str__, POW, __7c_str__, ROOT, __0c_str__));
constexpr const double foo=sqrt(4);
return 0;

}
