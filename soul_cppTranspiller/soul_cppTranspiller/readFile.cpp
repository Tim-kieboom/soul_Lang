#include "readFile.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int64_t getFileLineCount(const char* path)
{
    ifstream fileReader(path);

    if (!fileReader.is_open())
    {
        cout << "could not open sourceFile path: " << path << endl;
        return -1;
    }
    int64_t lineCount = count(istream_iterator<char>(fileReader >> noskipws), {}, '\n');
    fileReader.close();
    return lineCount;
}

string readFile_andStoreLine(const char* path, int64_t& lineCount)
{
    lineCount = getFileLineCount(path);

    ifstream fileReader(path);

    if (!fileReader.is_open())
    {
        cout << "could not open sourceFile path: " << path << endl;
        return string();
    }

    stringstream ss;
    string line;

    uint64_t counter = 1;
    while (getline(fileReader, line))
    {
        if (line.empty())
            ss << " \n" << counter << '\n';
        else
            ss << line << '\n' << counter << '\n';

        counter++;
    }
    ss << "\n\n" << counter;

    fileReader.close();
    return ss.str();
}

string readFile(const char* path, int64_t& lineCount)
{
    lineCount = getFileLineCount(path);

    ifstream fileReader(path);

    if (!fileReader.is_open())
    {
        cout << "could not open sourceFile path: " << path << endl;
        return string();
    }

    stringstream ss;
    string line;

    uint64_t counter = 1;
    while (getline(fileReader, line))
    {
        if (line.empty())
            ss << " \n";
        else
            ss << line << '\n';

        counter++;
    }
    ss << " \n";

    fileReader.close();
    return ss.str();
}