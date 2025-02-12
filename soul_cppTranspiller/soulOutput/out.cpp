#include "soul_hardCodedFunctions/soul_hardCodedFunctions.h"
constexpr const char* __0c_str__ = "log:";
constexpr const char* __1c_str__ = ", pow:";
constexpr const char* __2c_str__ = ", root";
constexpr const char* __3c_str__ = "";
int main(int __Soul_argc__, char** __Soul_argv__)
{
    __Soul_ARRAY__<__Soul_STR__> args = __Soul_ARRAY__<__Soul_STR__>(__Soul_argc__);
    for (uint64_t __CPP_i__ = 0; __CPP_i__ < __Soul_argc__; __CPP_i__++)
        args.__soul_UNSAFE_at__(__CPP_i__) = __Soul_argv__[__CPP_i__];
    println();
    int64_t argsSize = arrSize(args);
    int64_t i = -1;
    int8_t v1 = 10;
    int8_t v2 = 4;
    double log_ = __Soul_CompileConst_math__::CompileConst_log(100, 10);
    constexpr const double pow = __Soul_CompileConst_math__::CompileConst_pow(4, 0.5);
    constexpr const double root = __Soul_CompileConst_math__::CompileConst_root(4, 2);
    println(__soul_format_string__(__0c_str__, log_, __1c_str__, pow, __2c_str__, root, __3c_str__));
    return 0;

}
