#include "optab.h"

std::unordered_map<std::string, OpInfo> OPTAB =
{
    {"ADD",{0x18,3}},
    {"COMP",{0x28,3}},
    {"DIV",{0x24,3}},
    {"J",{0x3C,3}},
    {"JEQ",{0x30,3}},
    {"JGT",{0x34,3}},
    {"JLT",{0x38,3}},
    {"JSUB",{0x48,3}},
    {"LDA",{0x00,3}},
    {"LDB",{0x68,3}},
    {"LDCH",{0x50,3}},
    {"LDL",{0x08,3}},
    {"LDT",{0x74,3}},
    {"LDX",{0x04,3}},
    {"MUL",{0x20,3}},
    {"RD",{0xD8,3}},
    {"RSUB",{0x4C,3}},
    {"STA",{0x0C,3}},
    {"STB",{0x78,3}},
    {"STCH",{0x54,3}},
    {"STL",{0x14,3}},
    {"STT",{0x84,3}},
    {"STX",{0x10,3}},
    {"SUB",{0x1C,3}},
    {"TD",{0xE0,3}},
    {"TIX",{0x2C,3}},
    {"WD",{0xDC,3}},
    {"CLEAR",{0xB4,2}},
    {"COMPR",{0xA0,2}},
    {"TIXR",{0xB8,2}}
};

std::unordered_map<std::string, int> REGTAB =
{
    {"A",0},
    {"X",1},
    {"L",2},
    {"B",3},
    {"S",4},
    {"T",5},
    {"F",6}
};