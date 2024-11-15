#include "tokenizer.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "stringTools.h"

using namespace std;

static inline bool hasComments(char ch, char prevCh)
{
    return ch == '/' && prevCh == '/';
}

static inline bool removeComment(/*out*/ string& line)
{
    char prevPrevCh = -1;
    char prevCh = -1;

    bool inString = false;
    int64_t multiLineIndex = -1;
    int64_t prevEndLine = -1;

    uint64_t size = line.size();
    for (uint64_t i = 0; i < size; i++)
    {
        const char& ch = line.at(i);

        if (prevCh == '/' && ch == '*')
        {
            multiLineIndex = i;
        }
        else if (prevCh == '*' && ch == '/')
        {
            if (multiLineIndex == -1)
            {
                cout << "!!error!! '*/' without '/*'";
                return false;
            }

            string_removeSpan(line, multiLineIndex - 1, i + 1);
            uint64_t sizeLoss = size - line.size();
            size = line.size();
            i = i - sizeLoss;
            multiLineIndex = -1;
        }

        if (ch == '\"')
            inString = !inString;

        if (inString)
            continue;

        if (hasComments(ch, prevCh))
        {
            int64_t prevIndex = (int64_t)i - 1;
            uint32_t endOfComment = string_find(line, '\n', prevIndex).at(0);
            string_replaceAt(line, prevIndex, endOfComment, " \n");

            int64_t sizeLoss = size - line.size();
            size = line.size();
            i = (i == 0) ? 1 : endOfComment - sizeLoss;
            prevCh = ch;
            prevPrevCh = prevCh;
        }

        prevCh = ch;
        prevPrevCh = prevCh;
    }

    return true;
}

static inline bool storeRawString_inMap(/*out*/ string& sourceFile, /*out*/ MetaData& metaData)
{
    unordered_map<ui64_string, C_strPair>& constStringStore = metaData.c_strStore;
    if (string_count(sourceFile, '\"') % 2 == 1)
    {
        cout << "\" count is uneven" << endl;
        return false;
    }

    int64_t replaceOffset = 0;
    int64_t oldSize = sourceFile.size();
    vector<uint32_t> indexes = string_find(sourceFile, '\"');
    uint64_t c_strCounter = 0;

    unordered_map<uint64_t, const char*> rawStrings;
    for (uint64_t i = 0; i < indexes.size(); i += 2)
    {
        int64_t newSize;
        const uint64_t begin = indexes.at(i) + replaceOffset;
        const uint64_t end = indexes.at(i + 1) + replaceOffset;

        const char* rawStr = string_copyTo_c_str(sourceFile.substr(begin, end - begin + 1));
        uint64_t strHash = string_hash(rawStr);
        if (rawStrings.find(strHash) != rawStrings.end()) //if c_str already exists
        {
            string_replaceAt(sourceFile, begin, end + 1, rawStrings.at(strHash));
            newSize = sourceFile.size();
            replaceOffset = newSize - oldSize;
            continue;
        }

        stringstream ss;
        ss << "__" << c_strCounter++ << "c_str__";
        const char* c_strName = string_copyTo_c_str(ss.str());
        rawStrings.insert({ strHash, c_strName });


        string_replaceAt(sourceFile, begin, end + 1, c_strName);
        newSize = sourceFile.size();
        replaceOffset = newSize - oldSize;

        constStringStore.insert({ string_consexprHash(c_strName), {c_strName, rawStr} });
    }

    return true;
}

static inline bool checkBrackets(string& sourceFile)
{
    if (string_count(sourceFile, '(') != string_count(sourceFile, ')'))
    {
        cout << "uneven amount of open/closed round bracket '(' count: " << string_count(sourceFile, '(') << ", ')' count: " << string_count(sourceFile, ')') << endl;
        return false;
    }

    if (string_count(sourceFile, '{') != string_count(sourceFile, '}'))
    {
        cout << "uneven amount of open/closed curly bracket '{' count: " << string_count(sourceFile, '{') << ", '}' count: " << string_count(sourceFile, '}') << endl;
        return false;
    }

    if (string_count(sourceFile, '[') != string_count(sourceFile, ']'))
    {
        cout << "uneven amount of open/closed square bracket '[' count: " << string_count(sourceFile, '[') << ", ']' count: " << string_count(sourceFile, ']') << endl;
        return false;
    }

    return true;
}

static inline void tokenizeLine(const string& line, vector<Token>& tokenizer, uint64_t& lineNumber)
{
    vector<string> splitArgs = string_splitOn(line, { "(", ")", "{", "}", ":", ";", "++", "--", "!=", "+=", "-=", "/=", "*=", "+", "-", "/", "*", "=" });
    string argsLine = concatToString(splitArgs, " ");
    string_replace(/*out*/argsLine, ",", " ,");

    vector<string> tokens = string_split(argsLine, ' ');
    for (uint32_t i = 0; i < tokens.size(); i++)
    {
        string token = tokens.at(i);

        string_remove(token, ' ');
        if (!token.empty())
            tokenizer.emplace_back(token, lineNumber);
    }
}

vector<Token> tokenize(/*out*/ string& sourceFile, /*out*/ MetaData& metaData)
{
    vector<Token> tokenizer;
    tokenizer.reserve(string_count(sourceFile, ' '));

    string_replace(sourceFile, '\t', ' ');

    if (!removeComment(/*out*/sourceFile))
        return {};

    if (!storeRawString_inMap(/*out*/ sourceFile, /*out*/ metaData))
        return {};

    if (!checkBrackets(sourceFile))
        return {};

    vector<string> lines = string_split(sourceFile, '\n');
    for (uint32_t i = 0; i < lines.size(); i += 2)
    {
        string line = lines.at(i);
        string rawLineNumber = lines.at(i + 1);
        string_remove(line, '\n');
        string_remove(rawLineNumber, { '\n', ' ' });

        uint64_t lineNumber = stoul(rawLineNumber);

        tokenizeLine(line, /*out*/tokenizer, /*out*/lineNumber);
    }

    tokenizer.resize(tokenizer.size());
    return tokenizer;
}