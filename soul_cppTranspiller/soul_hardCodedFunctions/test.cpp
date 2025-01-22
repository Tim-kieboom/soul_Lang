#include "soul_hardCodedFunctions.h"

int main(int argc, char** argv) 
{
    __Soul_ARRAY__<__Soul_STR__> args(argc);
    for (int i = 0; i < argc; i++)
        args[i] = __Soul_STR__(argv[i]);

    auto newArgs = __Soul_Nullable__<__Soul_ARRAY__<__Soul_STR__>>();
    println(newArgs.has_value());
    println(newArgs.value());
    return 0;
}
