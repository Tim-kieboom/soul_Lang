#include "Type.h"
#include <sstream>
#include <unordered_map>
#include "Type.h"
#include "ClassInfo.h"
#include "TypeWrapper.h"
#include "PrimitiveType.h"
#include "stringTools.h"
#include "soulChecker.h"
using namespace std;

DuckType getDuckType(RawType& type)
{
    return getDuckType(getPrimitiveType(type.getType_WithoutWrapper()));
}

std::string toString(const RawType& type)
{
    stringstream ss;
    if (!type.isMutable)
        ss << "const ";
    ss << type.getType_WithoutWrapper();
    for (TypeWrapper wrap : type.typeWrappers)
    {
        ss << toString(wrap);
    }
    return ss.str();
}

static inline bool isClass(const std::string& token, std::unordered_map<std::string, ClassInfo>& classStore)
{
    return classStore.find(token) != classStore.end();
}

static inline bool isTemplateType(const std::string& token, std::map<std::string, TemplateType>& templateTypes)
{
    return templateTypes.find(token) != templateTypes.end();
}

Result<std::vector<std::string>> getTypeTokens(const std::string& stringedRawType, const uint32_t currentLine)
{
    string strType = stringedRawType;
    string_remove(strType, ' ');

    if (strType.empty())
        return ErrorInfo("type is <empty>", currentLine);

    vector<string> typeTokens = string_splitOn(strType, { "const", "[]", "*", "&" });

    //if first element is empty remove element
    if (typeTokens.front().empty())
        typeTokens.erase(typeTokens.begin());

    return typeTokens;
}

Result<RawType> getRawType_fromStringedRawType(const std::string& stringedRawType, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, const uint64_t currentLine)
{
    Result<vector<string>> typeTokensResult = getTypeTokens(stringedRawType, currentLine);
    if (typeTokensResult.hasError)
        return typeTokensResult.error;

    vector<string>& typeTokens = typeTokensResult.value();
    
    uint64_t i = 0;
    bool isMutable = true;
    RawType rawType;
    if (typeTokens.at(i) == "const")
    {
        isMutable = false;
        i++;
    }

    PrimitiveType type = getPrimitiveType(typeTokens.at(i));
    if (type == PrimitiveType::invalid && !isClass(typeTokens.at(i), classStore) && !isTemplateType(typeTokens.at(i), templateTypes))
    {
        return ErrorInfo("type: \'" + typeTokens.at(i) + "\', is not a reconized Type", currentLine);
    }

    rawType = RawType(typeTokens.at(i), isMutable);

    if (i == typeTokens.size()-1)
        return rawType;

    Result<void> result;
    uint8_t refrenceCounter = 0;
    while (i++ < typeTokens.size()-1)
    {
        TypeWrapper wrap = getTypeWrapper(typeTokens.at(i));
        if (wrap == TypeWrapper::invalid)
        {
            return rawType;
        }

        result = rawType.addTypeWrapper(wrap, currentLine);
        if (result.hasError)
            return result.error;
    }

    return rawType;
}

Result<RawType> getRawType_fromLiteralValue(const std::string& value, const uint64_t currentLine)
{
    PrimitiveType valueType = getPrimitiveType_fromValue(value);
    if (valueType == PrimitiveType::invalid)
        return ErrorInfo("value is not a LiteralValue", currentLine);

    std::string rawType = toString(valueType);

    return RawType(rawType, false);
}

static inline bool isTemplateType(std::string& token, std::unordered_map<std::string, TemplateType>& templateTypes)
{
    return templateTypes.find(token) != templateTypes.end();
}

Result<RawType> getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes)
{
	const uint64_t beginIndex = iterator.i;
	string& token = iterator.currentToken;
    bool isMutable = true;
    RawType rawType;

    if (token == "const")
    {
        isMutable = false;
        if (!iterator.nextToken())
        {
            iterator.at(beginIndex);
            return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
        }
    }

    PrimitiveType type = getPrimitiveType(token);
    if (type == PrimitiveType::invalid && !isClass(token, classStore) && !isTemplateType(token, templateTypes))
    {  
        iterator.at(beginIndex);
        return ErrorInfo("type: \'" + token + "\', is not a reconized Type", iterator.currentLine);
    }

    rawType = RawType(token, isMutable);

    Result<void> result;
    uint8_t refrenceCounter = 0;
    while(iterator.nextToken())
    {
        TypeWrapper wrap = getTypeWrapper(token);
        if (wrap == TypeWrapper::invalid)
        {
            if (!iterator.nextToken(/*step:*/-1))
                break;

            return rawType;
        }

        result = rawType.addTypeWrapper(wrap, iterator.currentLine);
        if (result.hasError)
            return result.error;
    }

    iterator.at(beginIndex);
    return ErrorInfo("unexpected end while trying to get TypeInfo", iterator.currentLine);
}
