#include "soul_hardCodedFunctions.h"

constexpr const char* __0c_str__="ellie";
constexpr const char* __1c_str__="log:";
constexpr const char* __2c_str__=", pow:";
constexpr const char* __3c_str__=", root";
constexpr const char* __4c_str__="";
int main(int __Soul_argc__, char** __Soul_argv__)
{
__Soul_ARRAY__<__Soul_STR__> args=__Soul_ARRAY__<__Soul_STR__>(__Soul_argc__);
for(uint64_t __CPP_i__ = 0; __CPP_i__ < __Soul_argc__; __CPP_i__++)
        args.__soul_UNSAFE_at__(__CPP_i__)=__Soul_argv__[__CPP_i__];
println();
uint64_t argsSize=arrSize(args);
int64_t i=-1;
int8_t v1=10;
int8_t v2=4;
double log_=__Soul_CompileConst_math__::root(__Soul_CompileConst_math__::pow(__Soul_CompileConst_math__::root(4, 2), __Soul_CompileConst_math__::log(100, 10)), 2);
constexpr const double pow=__Soul_CompileConst_math__::pow(4, 0.5);
constexpr const double root=__Soul_CompileConst_math__::root(4, 2);
__Soul_ARRAY__<__Soul_STR__> array;
array=__Soul_ARRAY__<__Soul_STR__>(10);
__Soul_ARRAY__<__Soul_STR__> range=array.__soul_makeSpan_fail__(__Soul_Range__(1, 3));
__Soul_ARRAY__<__Soul_STR__> range1=array.__soul_makeSpan_fail__(__Soul_Range__(3, __Soul_Range__::RangeType::END));
__Soul_ARRAY__<__Soul_STR__> range2=array.__soul_makeSpan_fail__(__Soul_Range__(1, __Soul_Range__::RangeType::START));
array[3]=__0c_str__;
println(array);
// __Soul_ARRAY__<int32_t> array2=__Soul_copy__(array, 20);
println(__soul_format_string__(__1c_str__, log_, __2c_str__, pow, __3c_str__, root, __4c_str__));
return 0;

}