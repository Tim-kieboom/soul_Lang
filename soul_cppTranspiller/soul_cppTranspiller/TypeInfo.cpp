#include "TypeInfo.h"
#include <sstream>
using namespace std;

string toString(const TypeInfo& type)
{
    if (!type.isValid())
        return "<invalid-type>";

    stringstream ss;
    if (type.isMutable)
        ss << "const ";

    if(type.isComplexType)
    {
        string name;
        const ComplexType_Info& info = type.complexType.info;
        switch(type.complexType.complexType_type)
        {
        case ComplexType_Type::classInfo:
            name = info.classInfo->className;
            break;

        default:
        case ComplexType_Type::invalid:
            name = "<invalid-ComplexType>";
            break;
        }

        ss << name;
    }
    else
    {
        ss << toString(type.primType);
    }

    for(TypeWrapper wrap : type.typeWrappers)
    {
        ss << toString(wrap);
    }

    return ss.str();
}

Result<TypeInfo> getTypeInfo(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore)
{
    const uint64_t beginIndex = iterator.i;
    string& token = iterator.currentToken;
    TypeInfo typeInfo;
    bool isMutable = true;

    if (token == "const")
    {
        isMutable = false;
        if (!iterator.nextToken())
        {
            iterator.at(beginIndex);
            return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
        }
    }

    PrimitiveType primType = getType(token);
    if (primType != PrimitiveType::invalid)
    {
        typeInfo = TypeInfo(primType, isMutable);
    }
    else if (classStore.find(token) != classStore.end())
    {
        typeInfo = TypeInfo(&classStore.at(token), isMutable);
    }
    else
    {
        iterator.at(beginIndex);
        return ErrorInfo("type: \'" + token + "\', is not a reconized Type", iterator.currentLine);
    }

    Result<void> result;
    vector<TypeWrapper>& typeWrapper = typeInfo.typeWrappers;
    while(iterator.nextToken())
    {
        if(token == "*")
        {
            result = typeInfo.addTypeWrapper(TypeWrapper::pointer, iterator.currentLine);
            if (result.hasError)
                return result.error;
        }
        else if(token == "&")
        {
            result = typeInfo.addTypeWrapper(TypeWrapper::refrence, iterator.currentLine);
            if (result.hasError)
                return result.error;
        }
        else if (token == "[]")
        {
            result = typeInfo.addTypeWrapper(TypeWrapper::array_, iterator.currentLine);
            if (result.hasError)
                return result.error;
        }
        else
        {
            if (!iterator.nextToken(/*step:*/-1))
                break;

            TypeWrapper lastWrapping = (typeInfo.typeWrappers.size() == 0) ? TypeWrapper::default_ : typeInfo.typeWrappers.back();
            switch(lastWrapping)
            {
            case TypeWrapper::array_:
                typeInfo.isArray = true;
                break;
            case TypeWrapper::pointer:
                typeInfo.isPointer = true;
                break;

            default:
            case TypeWrapper::invalid:
            case TypeWrapper::refrence:
            case TypeWrapper::default_:
                break;
            }

            return typeInfo;
        }
    }

    iterator.at(beginIndex);
    return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
}
