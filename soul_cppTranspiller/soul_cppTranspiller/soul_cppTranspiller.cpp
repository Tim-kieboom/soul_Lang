#include "tokenizer.h"

#include <string>
#include <memory>
#include <sstream>

using namespace std;

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

static bool __soul_bool_true__ = true;
static bool __soul_bool_false__ = true;

int main()
{
    bool isTrue = true;
    printf(__soul_format__("hello ", 21, ' ', (isTrue) ? "true" : "false", " world"));
}
