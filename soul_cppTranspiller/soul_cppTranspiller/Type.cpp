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

static inline void _stringTemplateType(const RawType& type, stringstream& ss) 
{
    ss << '<';

    for (size_t i = 0; i < type.templateDefines.size(); i++)
    {
        ss << toString(type.templateDefines.at(i));

        if (i + 1 < type.templateDefines.size())
            ss << ",";
    }

    ss << '>';
}

std::string toString(const RawType& type)
{
    stringstream ss;
    if (!type.isMutable)
        ss << "const ";
    ss << type.getType_WithoutWrapper();

    if (type.templateDefines.size() > 0)
        _stringTemplateType(type, /*out*/ss);

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

    vector<string> typeTokens = string_splitOn(strType, { "const", "[]", "*", "&", "<", ">", ","});

    //if element is empty remove element
    for (uint32_t i = 0; i < typeTokens.size(); i++)
    {
        string& token = typeTokens.at(i);
        if(token.empty())
            typeTokens.erase(typeTokens.begin() + i);
    }
    return typeTokens;
}

Result<RawType> getRawType_fromStringedRawType(const std::string& stringedRawType, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, const uint64_t currentLine)
{
    Result<vector<string>> typeTokensResult = getTypeTokens(stringedRawType, currentLine);
    if (typeTokensResult.hasError)
        return typeTokensResult.error;

    vector<string>& typeTokens_ = typeTokensResult.value();
    vector<Token> typeTokens;
    typeTokens.reserve(typeTokens_.size());
    for(string& str : typeTokens_)
    {
        typeTokens.push_back(Token(str, currentLine));
    }

    TokenIterator typeIterator = TokenIterator(typeTokens);
    if(!typeIterator.nextToken())
        return ErrorInfo("unexpected end while parsing type", currentLine);

    string& token = typeIterator.currentToken;


    bool isMutable = true;
    RawType rawType;
    if (token == "const")
    {
        isMutable = false;
        if(!typeIterator.nextToken())
            return ErrorInfo("unexpected end while parsing type", currentLine);
    }

    PrimitiveType type = getPrimitiveType(token);
    if (type == PrimitiveType::invalid && !isClass(token, classStore) && !isTemplateType(token, templateTypes))
    {
        return ErrorInfo("type: \'" + token + "\', is not a reconized Type", currentLine);
    }

    rawType = RawType(token, isMutable);

    if (typeIterator.i == typeIterator.size()-1)
        return rawType;

    string nextToken;
    typeIterator.peekToken(nextToken);

    if(nextToken == "<")
    {
        if (!typeIterator.nextToken())
            return ErrorInfo("unexpected end while parsing templateType ctor", currentLine);

        while(true)
        {
            if (!typeIterator.nextToken())
                return ErrorInfo("unexpected end while parsing templateType ctor", currentLine);

            Result<RawType> templateType = getRawType(typeIterator, classStore, templateTypes);
            if (templateType.hasError)
                return templateType.error;

            rawType.templateDefines.push_back(templateType.value());

            if (!typeIterator.nextToken())
                return ErrorInfo("unexpected edn while parsing templateType ctor", currentLine);

            if (token == ">")
                break;
            else if (token == ",")
                continue;
            else
                return ErrorInfo("\'" +token+ "\' is invalid in templateType ctor", currentLine);
        }
    }

    Result<void> result;
    uint8_t refrenceCounter = 0;
    while (typeIterator.nextToken())
    {
        TypeWrapper wrap = getTypeWrapper(token);
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

static inline Result<vector<RawType>> _getDefinedTemplateType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, RawType& rawType)
{
    vector<RawType> templateDefines;
    string& token = iterator.currentToken;
    Nullable<ClassInfo> classInfo = rawType.toClassInfo(classStore);

    if (classInfo.isNull())
        return ErrorInfo("you are defining template type (<TYPE>) but parent type: \'" + toString(rawType) + "\' is not a class", iterator.currentLine);

    while(iterator.nextToken())
    {
        Result<RawType> type = getRawType(iterator, classStore, templateTypes);
        if (type.hasError)
            return type.error;

        templateDefines.push_back(type.value());

        if(!iterator.nextToken())
            return ErrorInfo("unexpected end while trying to get Type", iterator.currentLine);

        if (token == ">")
            break;
        else if (token == ",")
            continue;
        else
            return ErrorInfo("\'" +token+ "\' is invalid in template type ctor", iterator.currentLine);
    }

    if (classInfo.value().templateTypes.size() != templateDefines.size())
    {
        stringstream ss;
        ss << "in type: \'" << toString(rawType) << "\', amount of template types defined: \'" << templateDefines.size() << "\' does not equal the amount of template types in class: \'" << rawType.toClassInfo(classStore).value().templateTypes.size() << "\'";
        return ErrorInfo(ss.str(), iterator.currentLine);
    }

    return templateDefines;
}

static inline Result<RawType> _getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, bool* isWrongType = nullptr)
{
    string& token = iterator.currentToken;
    bool isMutable = true;
    RawType rawType;

    if (token == "const")
    {
        isMutable = false;
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while trying to get Type", iterator.currentLine);
    }

    PrimitiveType type = getPrimitiveType(token);
    if (type == PrimitiveType::invalid && !isClass(token, classStore) && !isTemplateType(token, templateTypes))
        return ErrorInfo("type: \'" + token + "\', is not a reconized Type", iterator.currentLine);

    rawType = RawType(token, isMutable);

    string nextToken;
    if (!iterator.peekToken(nextToken))
        return ErrorInfo("unexpected end while trying to get Type", iterator.currentLine);

    if (nextToken == "<")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while trying to get Type", iterator.currentLine);

        Result<vector<RawType>> templateDefinesResult = _getDefinedTemplateType(iterator, classStore, templateTypes, rawType);
        if (templateDefinesResult.hasError)
        {
            if (isWrongType != nullptr)
                *isWrongType = true;

            return templateDefinesResult.error;
        }

        rawType.templateDefines = templateDefinesResult.value();
    }

    if (rawType.isClass(classStore))
    {
        ClassInfo classInfo = rawType.toClassInfo(classStore).value();
        if (classInfo.templateTypes.size() != rawType.templateDefines.size())
        {
            stringstream ss;
            ss << "in type: \'" << toString(rawType) << "\', amount of template types defined: \'" << rawType.templateDefines.size() << "\' does not equal the amount of template types in class: \'" << rawType.toClassInfo(classStore).value().templateTypes.size() << "\'";
            if (isWrongType != nullptr)
                *isWrongType = true;

            return ErrorInfo(ss.str(), iterator.currentLine);
        }
    }

    Result<void> result;
    uint8_t refrenceCounter = 0;
    while (iterator.nextToken())
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
        {
            if (isWrongType != nullptr)
                *isWrongType = true;

            return result.error;
        }
    }

    return ErrorInfo("unexpected end while trying to get Type", iterator.currentLine);
}

Result<RawType> getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes)
{
    const uint64_t beginIndex = iterator.i;
    Result<RawType> type = _getRawType(iterator, classStore, templateTypes);
    if (type.hasError)
    {
        iterator.goToIndex(beginIndex);
        return type.error;
    }

    return type;
}

Result<RawType> getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, bool& isWrongType)
{
    const uint64_t beginIndex = iterator.i;
    Result<RawType> type = _getRawType(iterator, classStore, templateTypes, &isWrongType);
    if(type.hasError)
    {
        iterator.goToIndex(beginIndex);
        return type.error;
    }

    return type;
}
