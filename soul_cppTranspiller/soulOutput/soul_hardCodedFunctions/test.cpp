#include "soul_hardCodedFunctions.h"

constexpr const char* __0c_str__="ellie";
constexpr const char* __1c_str__="log:";
constexpr const char* __2c_str__=", pow 4 ** 0.5=";
constexpr const char* __3c_str__=", root 2 </ 4=";
constexpr const char* __4c_str__="";
constexpr const uint32_t MAX_U32=__Soul_CompileConst_math__::pow(32, 2)-1;
int main(int __Soul_argc__, char** __Soul_argv__)
{
__Soul_ARRAY__<__Soul_STR__> args=__Soul_ARRAY__<__Soul_STR__>(__Soul_argc__);
for(uint64_t __CPP_i__ = 0; __CPP_i__ < __Soul_argc__; __CPP_i__++)
        args.__soul_UNSAFE_at__(__CPP_i__)=__Soul_argv__[__CPP_i__];
println();
char ch=47;
int32_t ch1=i32(ch);
char ch2=char_(ch1);
__Soul_ARRAY__<__Soul_STR__> array;
array=__Soul_ARRAY__<__Soul_STR__>(10);
array[3]=__0c_str__;
println(len(array[3]));
println(array);
__Soul_ARRAY__<__Soul_STR__> array2=__Soul_copy__(array, 20);
double log_=__Soul_CompileConst_math__::root(__Soul_CompileConst_math__::pow(__Soul_CompileConst_math__::root(4, 2), __Soul_CompileConst_math__::log(100, 10)), 2);
constexpr const double POW=__Soul_CompileConst_math__::pow(4, 0.5);
constexpr const double ROOT=__Soul_CompileConst_math__::root(4, 2);
println(__soul_format_string__(__1c_str__, log_, __2c_str__, POW, __3c_str__, ROOT, __4c_str__));
return 0;

}
