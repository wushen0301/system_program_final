#ifndef OPTAB_H
#define OPTAB_H

#include <string>
#include <unordered_map>

struct OpInfo
{
    int opcode;
    int format;
};

extern std::unordered_map<std::string, OpInfo> OPTAB;

extern std::unordered_map<std::string, int> REGTAB;

#endif