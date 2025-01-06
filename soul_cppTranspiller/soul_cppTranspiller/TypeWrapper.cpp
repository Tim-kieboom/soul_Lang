#include "TypeWrapper.h"

using namespace std;

TypeWrapper getTypeWrapper(const string& token)
{
    if (token == toString(TypeWrapper::array_))
        return TypeWrapper::array_;

    else if (token == toString(TypeWrapper::refrence))
        return TypeWrapper::refrence;

    else if (token == toString(TypeWrapper::pointer))
        return TypeWrapper::pointer;

    else if (token == toString(TypeWrapper::default_))
        return TypeWrapper::default_;
    
    return TypeWrapper::invalid;
}

string toString(TypeWrapper wrapper)
{
    switch (wrapper)
    {
    case TypeWrapper::array_:
        return "[]";

    case TypeWrapper::refrence:
        return "&";

    case TypeWrapper::pointer:
        return "*";

    case TypeWrapper::default_:
        return "";

    default:
    case TypeWrapper::invalid:
        return "<invalid>";
    }
}
