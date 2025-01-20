#include "tokenizer.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "stringTools.h"

using namespace std;

static inline bool hasComments(char ch, char prevCh)
{
    return ch == '/' && prevCh == '/';
}

enum class RemoveComments_States
{
    none,
    inMultiLineComment,
    inString 
};

struct CharIterator
{
    char& ch;
    string& str;
    int64_t index = -1;

    CharIterator(char& refChar, string& str)
        : str(str), ch(refChar)
    {
    }

    bool nextChar(int64_t step = 1)
    {
        if (index + step + 1 > (int64_t)str.size())
            return false;

        index += step;
        ch = str.at(index);
        return true;
    }

    bool setOffset(int64_t offset)
    {
        if (index + offset < 0 || index + offset > (int64_t)str.size())
            return false;

        index = index + offset;
        ch = str.at(index);
        return true;
    }
};

static inline Result<void*> removeComments(/*out*/ string& file)
{
    RemoveComments_States state = RemoveComments_States::none;
    uint32_t multiCommentStack = 0;
    int64_t multiLineBeginIndex = -1;
    int64_t prevEndLine = -1;

    uint64_t size = file.size();

    char it = -1;
    char prevIt = -1;
    char prevPrevIt = -1;
    CharIterator iterator(/*out*/it, file);
    while(iterator.nextChar())
    {
        switch(state)
        {
        case RemoveComments_States::none:
        {
            if (prevIt == '/' && it == '*')
            {
                multiCommentStack++;
                multiLineBeginIndex = iterator.index;
                state = RemoveComments_States::inMultiLineComment;
            }
            else if (prevIt == '/' && it == '/')
            {
                int64_t prevIndex = (int64_t)iterator.index - 1;
                uint32_t endOfComment = string_find(file, '\n', prevIndex).at(0);
                string_replaceAt(file, prevIndex, endOfComment, " \n");

                int64_t offset = (iterator.index == 0) ? 1 : endOfComment - size - file.size();
                iterator.setOffset(offset);
            }
            else if (it == '\"')
            {
                state = RemoveComments_States::inString;
            }
        }
        break;

        case RemoveComments_States::inString:
        {
            if (it == '\"' && prevIt != '\\')
                state = RemoveComments_States::none;
        }
        break;

        case RemoveComments_States::inMultiLineComment:
        {
            if (prevIt == '/' && it == '*')
                multiCommentStack++;

            if (prevIt == '*' && it == '/')
            {
                multiCommentStack--;
                
                if(multiLineBeginIndex < 0)
                    return ErrorInfo("!!error!! tokenizer: '*/' without any '/*'", iterator.index);

                if (multiCommentStack != 0)
                    continue;

                if (!string_removeSpan(file, multiLineBeginIndex - 1, iterator.index + 1))
                    return ErrorInfo("string_removeSpan failed", iterator.index);

                uint64_t sizeLoss = size - file.size();
                size = file.size();
                iterator.index = iterator.index - sizeLoss;
                multiLineBeginIndex = -1;
                state = RemoveComments_States::none;
            }
        }
        break;

        }

        prevIt = it;
        prevPrevIt = prevIt;
    }

    return {};
}

static inline int64_t findEndOfStringIndex(const string& str, uint32_t offset)
{
    uint32_t safetyCounter = 0;
    while (safetyCounter++ < str.size())
    {
        int64_t qouteInString = string_findFirst(str, "\\\"", offset);
        int64_t stringEnd = string_findFirst(str, "\"", offset);

        if (qouteInString == stringEnd)
            offset = (uint32_t)qouteInString+1;
        else
            return stringEnd;
    }

    return -1;
}

static inline Result<void*> checkBrackets(const string& sourceFile)
{
    stringstream ss;

    if (string_count(sourceFile, '(') != string_count(sourceFile, ')'))
    {
        ss << "uneven amount of open/closed round bracket '(' count: " << string_count(sourceFile, '(') << ", ')' count: " << string_count(sourceFile, ')');
        return ErrorInfo(ss.str(), 0);
    }

    if (string_count(sourceFile, '{') != string_count(sourceFile, '}'))
    {
        ss << "uneven amount of open/closed curly bracket '{' count: " << string_count(sourceFile, '{') << ", '}' count: " << string_count(sourceFile, '}');
        return ErrorInfo(ss.str(), 0);
    }

    if (string_count(sourceFile, '[') != string_count(sourceFile, ']'))
    {
        ss << "uneven amount of open/closed square bracket '[' count: " << string_count(sourceFile, '[') << ", ']' count: " << string_count(sourceFile, ']');
        return ErrorInfo(ss.str(), 0);
    }

    return {};
}

static inline vector<uint32_t> getStringQoutePositions(const string& sourceFile)
{
    vector<uint32_t> indexes = string_find(sourceFile, '\"');

    vector<uint32_t> _inStringIndexes = string_find(sourceFile, "\\\"");
    unordered_set<uint32_t> inStringIndexes(_inStringIndexes.begin(), _inStringIndexes.end());

    uint64_t size = indexes.size();
    for (uint32_t i = 0; i < size; i++)
    {
        if (inStringIndexes.find(indexes.at(i) - 1) != inStringIndexes.end())
        {
            indexes.erase(indexes.begin() + i);
            i--;
            size--;
        }
    }

    return indexes;
}

static inline Result<void*> formatString(const string& rawFormatString, /*out*/ string& sourceFile)
{
    string formatedString = rawFormatString;

    //remove f from f""
    formatedString.at(0) = ' ';

    Result<void*> brackerResult = checkBrackets(formatedString);
    if (brackerResult.hasError)
        return brackerResult.error;

    uint64_t oldSize = sourceFile.size();
    string_replace(formatedString, "{", "\", ");
    string_replace(formatedString, "}", ", \"");

    formatedString = "__soul_format_string__(" + formatedString + ')';

    uint32_t replaceCount = string_replace(sourceFile, rawFormatString, formatedString);
    if (replaceCount == 0)
        return ErrorInfo("formatString failed to replace old string to formated string", string_findFirst(sourceFile, formatedString));

    return {};
}

static inline Result<void*> convertFormatStrings(/*out*/ string& sourceFile)
{
    uint32_t amountOfqouts = string_count(sourceFile, "\"") - string_count(sourceFile, "\\\"");
    if (amountOfqouts % 2 == 1)
       return ErrorInfo("\" count is uneven", 0);
    
    vector<uint32_t> indexes = getStringQoutePositions(sourceFile);
    unordered_set<string> rawFormatStrings;

    for (uint64_t i = 0; i < indexes.size(); i += 2)
    {
        int64_t index = indexes.at(i);
        if (index <= 0)
            continue;

        if (sourceFile.at(index - 1) != 'f')
            continue;


        int64_t endOfStringIndex = findEndOfStringIndex(sourceFile, (uint32_t)index + 2);
        if (endOfStringIndex < 0)
            return ErrorInfo("could not find endOfStringIndex while tokenizing string format", index);

        string formatString = sourceFile.substr(index - 1, endOfStringIndex - index + 2);

        if(rawFormatStrings.find(formatString) == rawFormatStrings.end())
            rawFormatStrings.insert(formatString);
    }

    for(const string& rawString : rawFormatStrings)
        formatString(rawString, /*out*/sourceFile);

    return {};
}

static inline Result<void*> storeRawString_inMap(/*out*/ string& sourceFile, /*out*/ MetaData& metaData)
{
    unordered_map<string, C_strPair>& constStringStore = metaData.c_strStore;
    uint64_t amountOfqouts = (uint64_t)string_count(sourceFile, "\"") - (uint64_t)string_count(sourceFile, "\\\"");
    if (amountOfqouts % 2 == 1)
        return ErrorInfo("\" count is uneven", 0);

    int64_t replaceOffset = 0;
    int64_t oldSize = sourceFile.size();

    vector<uint32_t> indexes = getStringQoutePositions(sourceFile);

    uint64_t c_strCounter = 0;

    unordered_map<string, string> rawStrings;
    for (uint64_t i = 0; i < indexes.size(); i += 2)
    {
        int64_t newSize;
        const uint64_t begin = indexes.at(i) + replaceOffset;
        const uint64_t end = indexes.at(i + 1) + replaceOffset;

        string rawStr = sourceFile.substr(begin, end - begin + 1);
        if (rawStrings.find(rawStr) != rawStrings.end()) //if c_str already exists
        {
            if(!string_replaceAt(sourceFile, begin, end + 1, rawStrings.at(rawStr)))
                return ErrorInfo("string_replaceAt failed while tokenizer is storing strings", begin);

            newSize = sourceFile.size();
            replaceOffset = newSize - oldSize;
            continue;
        }

        stringstream ss;
        ss << "__" << c_strCounter++ << "c_str__";
        string c_strName = ss.str();
        rawStrings.insert({ rawStr, c_strName });

        if(!string_replaceAt(sourceFile, begin, end + 1, c_strName))
            return ErrorInfo("string_replaceAt failed while tokenizer is storing strings", begin);

        newSize = sourceFile.size();
        replaceOffset = newSize - oldSize;

        constStringStore[c_strName] = {string_copyTo_c_str(c_strName), string_copyTo_c_str(rawStr)};
    }

    return {};
}

static inline void tokenizeDot(const string& token, vector<Token>& tokenizer, const uint64_t& lineNumber)
{
    vector<string> tokenArgs = string_splitOn(token, { "..", "." });

    for (auto& str : tokenArgs)
    {
        string_remove(str, ' ');
        if (!str.empty())
            tokenizer.emplace_back(str, lineNumber);
    }
}

static inline void tokenizeLine(const string& line, vector<Token>& tokenizer, uint64_t& lineNumber)
{
    static initializer_list<const char*> parseStrings = { "</", "**", "&", ",", "!=", "!", "[]", "[", "]", "(", ")", "{", "}", ":", ";", "++", "--", ">=", "<=", "==", "+=", "-=", "/=", "*=", "<", ">", "+", "-", "/", "*", "=" };

    vector<string> splitArgs = string_splitOn(line, parseStrings);
    string argsLine = concatToString(splitArgs, " ");

    vector<string> tokens = string_split(argsLine, ' ');
    for (uint32_t i = 0; i < tokens.size(); i++)
    {
        string token = tokens.at(i);
        if (string_contains(token, {'.'}) && getRawType_fromLiteralValue(token, lineNumber).hasError)
        {
            tokenizeDot(token, /*out*/tokenizer, lineNumber);
            continue;
        }

        string_remove(token, ' ');
        if (!token.empty())
            tokenizer.emplace_back(token, lineNumber);
    }
}

Result<vector<Token>> tokenize(/*out*/ string& sourceFile, /*out*/ MetaData& metaData)
{
    try
    {
        vector<Token> tokenizer;
        tokenizer.reserve(string_count(sourceFile, ' '));

        Result<void*> result;
        result = removeComments(/*out*/sourceFile);
        if (result.hasError)
            return result.error;

        result = convertFormatStrings(/*out*/ sourceFile);
        if (result.hasError)
            return result.error;

        result = storeRawString_inMap(/*out*/ sourceFile, /*out*/ metaData);
        if (result.hasError)
            return result.error;

        result = checkBrackets(sourceFile);
        if (result.hasError)
            return result.error;

        bool isInvis = false;

        string_replace(sourceFile, '\t', ' ');
        vector<string> lines = string_split(sourceFile, '\n');
        for (uint32_t i = 0; i < lines.size(); i += 2)
        {
            string line = lines.at(i);
            if (string_containsMultiple(line, { "#invis", "# invis" }))
                isInvis = true;

            if (string_containsMultiple(line, { "#endInvis", "# endInvis" }))
            {
                isInvis = false;
                continue;
            }

            if (isInvis)
                continue;

            string rawLineNumber = lines.at(i + 1);
            string_remove(line, '\n');
            string_remove(rawLineNumber, { '\n', ' ' });

            uint64_t lineNumber = stoul(rawLineNumber);

            tokenizeLine(line, /*out*/tokenizer, /*out*/lineNumber);
        }

        tokenizer.resize(tokenizer.size());
        return tokenizer;
    }
    catch(exception ex)
    {
        return ErrorInfo(string("!!exception thrown!! ") + ex.what(), 0);
    }
}