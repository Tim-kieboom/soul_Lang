#include "TypeInfo.h"
#include <sstream>
using namespace std;

string toString(TypeInfo& type)
{
    if (!type.isValid())
        return "<invalid-type>";

    stringstream ss;
    if (type.isMutable)
        ss << "const ";

    if(type.isComplexType)
    {
        ss << type.complexType.info.classInfo->className;
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
    string& token = iterator.currentToken;
    TypeInfo typeInfo;
    bool isMutable = true;

    if (token == "const")
    {
        isMutable = false;
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
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
        else
        {
            if (!iterator.nextToken(/*step:*/-1))
                break;

            return typeInfo;
        }
    }
    return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
}
