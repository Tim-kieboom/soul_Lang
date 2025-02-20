#include <array>
#include <string>
#include <memory>
#include <cstdio>
#include <time.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "Token.h"
#include "Result.h"
#include "MetaData.h"
#include "readFile.h"
#include "tokenizer.h"
#include "stringTools.h"
#include "convertToCpp.h"
#include "getAbstractSyntaxTree.h"

using namespace std;

struct RunOptions
{
    bool printTokenizer = true;
    bool printAST = true;
};

void printTokenizer(const string& sourceFile, const vector<Token>& tokens, const unordered_map<string, C_strPair>& constStringStore, clock_t start, clock_t end)
{
    cout << "\n-------------- reduced SourceFile --------------\n\n";
    vector<string> lines = string_split(sourceFile, '\n');
    for (uint32_t i = 0; i < lines.size(); i += 2)
    {
        string& line = lines.at(i);
        if (line == " ")
            continue;

        cout << line << '\n';
    }

    cout << "\n-------------- Tokens --------------\n\n";
    for (const Token& token : tokens)
        cout << token.text << '\n';

    cout << "\n-------------- strStore --------------\n\n";
    for (const auto& pair : constStringStore)
        cout << pair.second.name << " = " << pair.second.value << "\n";
    cout << endl;

    cout << "\n----------------------------------------------------------\n\n";
    cout << "tokenize:\n";
        cout << "\treduced file length: " << sourceFile.size() << " chars" << '\n';
        cout << "\tamount of tokens: " << tokens.size() << " tokens" << '\n';
        cout << "\ttokenize time: " << (end - start) << "ms" << '\n';

    cout << "\n----------------------------------------------------------\n" << endl;
}

static inline void _tokenizerGetErrorLine(string& sourceFile, Result<vector<Token>>& tokensResult)
{
    int64_t charNumber = tokensResult.error.lineNumber;
    string str = sourceFile.substr(0, charNumber);
    vector<uint32_t> lineIndex = string_find(str, "\n");
    uint64_t lineNumber = lineIndex.size() / 2;

    cout << sourceFile << endl;
    cout << "tokenizeError: " << tokensResult.error.message << ", charIndex: " << charNumber << ", prediced lineNumber: " << lineNumber;
}

pair<vector<Token>, MetaData> tokenize(string& sourceFile, RunOptions& option)
{
    MetaData metaData;
    clock_t start = clock();
    Result<vector<Token>> tokensResult = tokenize(/*out*/ sourceFile, /*out*/metaData);
    clock_t end = clock();
    if (tokensResult.hasError)
    {
        _tokenizerGetErrorLine(sourceFile, tokensResult);
        exit(1);
    }
    vector<Token> tokens = tokensResult.value();

    if(option.printTokenizer)
        printTokenizer(sourceFile, tokens, metaData.c_strStore, start, end);
    
    return pair<vector<Token>, MetaData>(move(tokens), metaData);
}

void transpile(vector<Token>& tokens, MetaData& metaData, RunOptions& option)
{
    clock_t start;
    clock_t end;

    if (option.printAST)
        cout << "\n----------------------------- Abstract_Syntax_Tree -----------------------------\n\n";

    start = clock();
    Result<SyntaxTree> syntaxTreeResult = getAbstractSyntaxTree(TokenIterator(tokens), metaData);
    end = clock();
    if (syntaxTreeResult.hasError)
    {
        cout << "transpillerError: \n" << syntaxTreeResult.error.message << ", onLine: " << syntaxTreeResult.error.lineNumber;
        exit(1);
    }

    SyntaxTree syntaxTree = move(syntaxTreeResult.value());
    if (option.printAST)
    {
        syntaxTree.print();
        cout << "\n----------------------------------------------------------\n\n";
        cout << "getAbstractSyntaxTree:\n";
            cout << "\tsyntaxTree amount of globalVariables:\t" << syntaxTree.globalVariables.size() << '\n';
            cout << "\tsyntaxTree amount of funcs/classes:\t" << syntaxTree.funcsAndClasses.size() << '\n';
            cout << "\tconvert to syntaxTree time:\t\t" << (end - start) << "ms" << '\n';

        cout << "\n----------------------------------------------------------\n" << endl;
    }

    start = clock();
    Result<string> cppFile = convertToCpp(syntaxTree, metaData);
    end = clock();
    if (cppFile.hasError)
    {
        cout << "cppConverterError: \n" << cppFile.error.message << ", onLine: " << cppFile.error.lineNumber;
        exit(1);
    }

    if (option.printAST)
    {

        cout << "\n----------------------------- CPP_FILE -----------------------------\n\n";
        cout << cppFile.value();

        cout << "\n----------------------------------------------------------\n\n";
            cout << "getAbstractSyntaxTree:\n";
            cout << "convert to cpp:\t\t" << (end - start) << "ms" << endl;
        cout << "\n----------------------------------------------------------\n" << endl;
    }
    //ofstream fileWriter(outputPath);
    //fileWriter << "#include \"soul_hardCodedFunctions.h\"" << cppFile.value();
    //fileWriter.close();

    //string execCppCodeCommand = "g++ " + string(outputPath);
    //execAndPrint(execCppCodeCommand.c_str());
    //string output = execAndPrint("a.exe");
    //cout << output << endl;

}

const char* selectArg(int argc, char* argv[], RunOptions& option)
{
    constexpr const char* test_Path = "C:\\Users\\tim_k\\OneDrive\\Documenten\\GitHub\\hobby\\soul_Lang\\soul_cppTranspiller\\soul_cppTranspiller\\quickTest.soul";

    const char* path;
    const char* outputPath = "out.cpp";

    if (argc == 1)
    {
        path = test_Path;
    }
    else
    {
        path = argv[1];
    }

    return path;
}

int main(int argc, char* argv[])
{
#ifdef NDEBUG
    try
    {
#endif

    RunOptions option;
    const char* path = selectArg(argc, argv, /*out*/option);

    int64_t lineCount;
    string sourceFile = readFile_andStoreLine(path, /*out*/lineCount);
    if (lineCount <= 0)
    {
        cout << "!!error!! readFile() lineCount: " << lineCount << endl;
        exit(1);
    }

    //printFile(sourceFile);

    pair<vector<Token>, MetaData> tokenResult = tokenize(sourceFile, option);
    vector<Token>& tokens = tokenResult.first;
    MetaData& metaData = tokenResult.second;

    transpile(tokens, metaData, option);

#ifdef NDEBUG
    }
    catch (exception ex)
    {
        cout << "\nexeption from transpiller: " << ex.what() << endl;
        exit(1);
    }
#endif
}
