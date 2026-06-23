#include "assembler.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

int baseRegister = 0;
bool baseSet = false;

static string trim(string s)
{
    while (!s.empty() && isspace(s.front()))
        s.erase(s.begin());

    while (!s.empty() && isspace(s.back()))
        s.pop_back();

    return s;
}

void Assembler::readSource(const string& filename)
{
    ifstream fin(filename);

    if (!fin)
    {
        cout << "Cannot open file\n";
        return;
    }

    string line;
    int lineNumber = 1;

    while (getline(fin, line))
    {
        SourceLine s;
        s.lineNumber = lineNumber++;
        s.originalLine = line;

        if (line.empty())
        {
            sourceLines.push_back(s);
            continue;
        }

        if (line[0] == '.')
        {
            s.comment = true;

            sourceLines.push_back(s);

            continue;
        }

        if (line.size() < 44)
            line.resize(44, ' ');

        s.label = trim(line.substr(0, 9));
        s.opcode = trim(line.substr(12, 9));
        s.operand = trim(line.substr(24, 20));

        sourceLines.push_back(s);
    }
}

void Assembler::printSourceLines()
{
    cout << "\n===============================\n";
    cout << "LOC\tLABEL\tOPCODE\tOPERAND\n";
    cout << "===============================\n";

    for (auto& line : sourceLines)
    {
        if (line.comment)
            continue;

        cout << toHex(line.loc, 4) << "\t";
        cout << line.label << "\t";
        cout << line.opcode << "\t";
        cout << line.operand << "\n";
    }
}

void Assembler::printPass2()
{
    cout << "=OBJECT CODES=\n";

    int idx = 1;

    for (auto& line : sourceLines)
    {
        if (line.comment)
            continue;

        cout << dec <<idx++ << " " << "L" << toHex(line.loc, 4) << " ";

		//no object code
        /*if (line.objectCode.empty())
        {
            cout << "null\n";
            continue;
        }*/

        //word
        if (line.format == "WORD")
        {
            cout << "WORD, obj=" << line.objectCode << "\n";
            continue;
        }
        //byte
        else if (line.format == "BYTE")
        {
            cout << "BYTE, obj=" << line.objectCode<< "\n";
            continue;
        }

        // format 2
        if (line.format == "format2")
        {
            cout << line.format << ", obj=" << line.objectCode << "\n";
            continue;
        }

        // format 3
        if (line.format == "format3" || line.format == "format4")
        {
            cout << line.format << ", "
                << line.addrModeName << ", "
                << "nixbpe=" << line.nixbpe << ", "
                << line.mode << ", "
                << "obj=" << line.objectCode
                << "\n";
            continue;
        }

        // fallback
        cout << "null\n";
    }
}