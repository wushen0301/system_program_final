#pragma once

#include <string>
#include <unordered_map>

using namespace std;

class Loader
{
private:

    unordered_map<int, unsigned char> memory;
    //unordered_map<string, int> symtab;

    string programName;
    int startAddress;   // object code裡面的起始address
    int programLength;
    int executionAddress;
    int loadAddress;    //實際被載入的start address

public:

    Loader();

	void setLoadAddress(int addr);

    void load(const string& filename);  //load object file

    void printMemory();
};
