#include "TypeWrapper.h"
using namespace std;

std::string toString(TypeWrapper wrapper)
{
    switch(wrapper)
    {
    case TypeWrapper::array_:
        return "[]";
    case TypeWrapper::refrence:
        return "&";

    case TypeWrapper::pointer:
        return "*";

    default:
    case TypeWrapper::invalid:
    case TypeWrapper::default_:
        return "";
    }
}
