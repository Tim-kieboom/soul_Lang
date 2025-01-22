#include "subLib/soul_print.hpp"
#include "subLib/soul_Fail_throw.hpp"
#include "subLib/soul_Nullable.hpp"

/*
<cpp args -> soul args code>

    __Soul_ARRAY__<__Soul_STR__> args(argc);
    for (int i = 0; i < argc; i++)
        args[i] = __Soul_STR__((const char*)argv[i]);
*/
