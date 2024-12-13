#include <array>
#include <string>
#include <memory>
#include <cstdio>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "readFile.h"
#include "tokenizer.h"
#include "transpiller.h"

using namespace std;

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

#ifdef _WIN32
    #define popen _popen
    #define pclose _pclose
#elif defined(__APPLE__) || defined(__linux__)
    #include <cstdio>
#else
    #error "Unsupported platform"
#endif

static string execAndPrint(const char* cmd)
{
    if (cmd == nullptr)
        return nullptr;

    string result;

    try
    {
        vector<char> buffer(128);
        unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe)
            throw std::runtime_error("popen() failed!");

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
        {
            result += buffer.data();
            if (result.size() > 1000000) // Arbitrary limit to prevent excessive memory usage
            {  
                throw std::runtime_error("Output too large");
            }
        }
    }
    catch (const std::exception ex)
    {
        return "execute: " + string(cmd) + ", failed" + string(ex.what());
    }

    return result;
}

constexpr const char* test_Path = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\soul_Lang\\soul_cppTranspiller\\soul_cppTranspiller\\Source.soul";
constexpr const char* test_outputPath = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\soul_Lang\\soul_cppTranspiller\\soulOutput\\out.cpp";
constexpr const char* test_hardCodedPath = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\soul_Lang\\soul_cppTranspiller\\soul_hardCodedFunctions\\soul_hardCodedFunctions.cpp";

int main(int argc, char* argv[])
{
    const char* path;
    const char* outputPath = "out.cpp";
    const char* hardCodedPath = "soul_hardCodedFunctions.cpp";

    if (argc == 1)
    {
        path = test_Path;
        outputPath = test_outputPath;
        hardCodedPath = test_hardCodedPath;
    }
    else
    {
        path = argv[1];
    }



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
    Result<vector<Token>> tokensResult = tokenize(/*out*/ sourceFile, /*out*/metaData);
    if (tokensResult.hasError)
    {
        int64_t charNumber = tokensResult.error.lineNumber;
        string str = sourceFile.substr(0, charNumber);
        vector<uint32_t> lineIndex = string_find(str, "\n");
        uint64_t lineNumber = lineIndex.size() / 2;

        cout << sourceFile << endl;
        cout << "tokenizeError: " << tokensResult.error.message << ", charIndex: " << charNumber << ", prediced lineNumber: " << lineNumber;
        exit(1);
    }
    vector<Token> tokens = tokensResult.value();

    printTokenizer(sourceFile, tokens, metaData.c_strStore);
#ifdef NDEBUG
    try
    {
#endif
        metaData.transpillerOption = TranspilerOptions();
        //metaData.transpillerOption.addEndLines = false;
        Result<string> result = transpileToCpp(tokens, /*out*/ metaData);
        if (result.hasError)
        {
            cout << "transpillerError: " << result.error.message << ", onLine: " << result.error.lineNumber;
            exit(1);
        }

        ofstream fileWriter(outputPath);
        fileWriter << metaData.getCpptIncludes() << hardCodeLib << result.value();
        fileWriter.close();

#ifdef NDEBUG
        this_thread::sleep_for(100ms);
#endif

        string execCppCodeCommand = "g++ " + string(outputPath);
        execAndPrint(execCppCodeCommand.c_str());
        string output = execAndPrint("a.exe");
        cout << output << endl;

#ifdef NDEBUG
    }
    catch (exception ex)
    {
        cout << "\nexeption from transpiller: " << ex.what() << endl;
        exit(1);
    }
#endif
}
