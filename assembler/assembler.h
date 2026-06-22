#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

extern int baseRegister;
extern bool baseSet;

struct Symbol
{
    int value;
    bool relative;  // true = R, false = A

    Symbol(int v = 0, bool r = true)
        : value(v), relative(r)
    {
    }
};

struct SourceLine
{
    int lineNumber;
    int loc;
    std::string label;
    std::string opcode;
    std::string operand;
    bool comment;
    std::string originalLine;
    std::string objectCode;

    // for pass 2's info
    std::string format;          // format2 / format3 / format4
    std::string mode;            // Simple / Immediate / Indirect
    std::string addrModeName;    // pc-relative / base-relative
    std::string nixbpe;          // ex: 110010 

    SourceLine()
    {
        lineNumber = 0;
        loc = 0;
        comment = false;
    }
};

class Assembler
{
public:

    void readSource(const std::string& filename);
    void printSourceLines();
    void pass1();
    void pass2();
    void printSymtab();
	void printPass2();
    void generateObjectProgram();
    void printObjectProgram();
    void writeObjectFile(const std::string& filename);
    void writeSymtab(const std::string& filename);

protected:

    std::vector<SourceLine> sourceLines;
    std::unordered_map<std::string, Symbol> symtab;
    int startAddress = 0;
    int programLength = 0;

    std::vector<std::string> textRecords;
    std::vector<std::string> modificationRecords;
    std::string headerRecord;
    std::string endRecord;
};

#endif
