
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <fstream>

#include "readFile.h"
#include "tokenizer.h"
#include "transpiller.h"

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

void printFile(const string& file)
{
    cout << "\n-------------- RawSourceFile --------------\n\n";
    uint64_t lineCounter = 1;
    vector<string> lines = string_split(file, '\n');
    for (uint32_t i = 0; i < lines.size(); i += 2)
        cout << lineCounter++ << ". " << lines.at(i) << '\n';
}

void printTokenizer(const string& sourceFile, const vector<Token>& tokens, const unordered_map<string, C_strPair>& constStringStore)
{
    cout << "\n-------------- reduced SourceFile --------------\n\n";
    vector<string> lines = string_split(sourceFile, '\n');
    for (uint32_t i = 0; i < lines.size(); i += 2)
        cout << lines.at(i) << '\n';

    cout << "\n-------------- Tokens --------------\n\n";
    for (const Token& token : tokens)
        cout << token.text << '\n';

    cout << "\n-------------- strStore --------------\n\n";
    for (const auto& pair : constStringStore)
        cout << pair.second.name << " = " << pair.second.value << "\n";
    cout << endl;
}

int main()
{
    const char* path = "Source.soul";
    const char* hardCodedPath = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\tkang\\tkang\\hardCodedFunctions.cpp";
    const char* outputPath = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\soul_Lang\\soul_cppTranspiller\\soulOutput\\out.cpp";

    int64_t lineCount;
    int64_t libLineCount;
    string sourceFile = readFile_andStoreLine(path, lineCount);
    string hardCodeLib = readFile(hardCodedPath, libLineCount);
    if (lineCount <= 0 || libLineCount <= 0)
    {
        cout << "!!error!! readFile() lineCount: " << lineCount << ", libLineCount: " << libLineCount << endl;
        exit(1);
    }

    printFile(sourceFile);

    MetaData metaData;
    vector<Token> tokens = tokenize(/*out*/ sourceFile, /*out*/metaData);
    if (tokens.empty())
    {
        cout << "\n-------------- Tokens --------------\n\n";
        cout << "!warning! tokenizer empty\n";
        exit(1);
    }

    printTokenizer(sourceFile, tokens, metaData.c_strStore);

    try
    {
        Result<string> result = transpileToCpp(tokens, TranspilerOptions(), /*out*/ metaData);
        if (result.hasError)
        {
            cout << result.error.message << ", onLine: " << result.error.lineNumber;    
        #ifdef NDEBUG
            exit(1);
        #endif
        }

        ofstream fileWriter(outputPath);
        fileWriter << metaData.getCpptIncludes() << result.value();
        fileWriter.close();
    }
    catch (exception ex)
    {
        cout << "\nexeption from transpiller: " << ex.what() << endl;
        exit(1);
    }
}
