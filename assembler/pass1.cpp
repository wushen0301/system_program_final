#include "assembler.h"
#include "optab.h"
#include "utils.h"
#include "expression.h"

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

//計算BYTE的長度
static int byteLength(const string& operand)
{
    if (operand.size() < 3)
        return 1;

    if (operand[0] == 'C')
    {
        return operand.size() - 3;
    }

    if (operand[0] == 'X')
    {
        return (operand.size() - 3) / 2;
    }

    return 1;
}

// 計算指令的長度
static int instructionLength(const string& opcode)
{
    if (opcode.empty())
        return 0;

    if (opcode[0] == '+')
        return 4;

    auto it = OPTAB.find(opcode);

    if (it != OPTAB.end())
        return it->second.format;

    return 0;
}

void Assembler::pass1()
{
    int LOCCTR = 0;
    bool started = false;

    for (auto& line : sourceLines)
    {
        if (line.comment)
            continue;

        if (line.label == "BASE")
            continue;

        // START
        if (line.opcode == "START")
        {
            if (!line.operand.empty())
            {
                startAddress = stoi(line.operand, nullptr, 16);
            }
            else
            {
                startAddress = 0;
            }

            LOCCTR = startAddress;
            line.loc = LOCCTR;
            started = true;

            //Program Name(R)
            if (!line.label.empty())
            {
                symtab[line.label] = Symbol(LOCCTR, true);
            }

            continue;
        }

        //if no start
        if (!started)
        {
            startAddress = 0;
            LOCCTR = 0;
            started = true;
        }

        //記錄 LOC
        line.loc = LOCCTR;

        // SYMBOL TABLE (NON-EQU ONLY)
        if (!line.label.empty() && line.opcode != "EQU")
        {
            if (symtab.find(line.label) != symtab.end())
            {
                cout << "[ERROR] Duplicate Symbol : "
                    << line.label
                    << " at line "
                    << line.lineNumber
                    << endl;
            }
            else
            {
                symtab[line.label] = Symbol(LOCCTR, true); // R
            }
        }

        // OPCODES
        int instLength = instructionLength(line.opcode);

        if (instLength > 0)
        {
            LOCCTR += instLength;
        }
        else if (line.opcode == "WORD")
        {
            LOCCTR += 3;
        }
        else if (line.opcode == "RESW")
        {
            LOCCTR += stoi(line.operand) * 3;
        }
        else if (line.opcode == "RESB")
        {
            LOCCTR += stoi(line.operand);
        }
        else if (line.opcode == "BYTE")
        {
            LOCCTR += byteLength(line.operand);
        }
        else if (line.opcode == "BASE")
        {
            baseRegister = symtab[line.operand].value;
            baseSet = true;
            continue;
        }
        else if (line.opcode == "END")
        {
            break;
        }

        // EQU
        else if (line.opcode == "EQU")
        {
            EvalResult res = Expression::evaluate(line.operand, symtab);

            if (res.error)
            {
                cout << "[ERROR] EQU failed at line " << line.lineNumber << endl;
                continue;
            }

            symtab[line.label] = Symbol(res.value, res.relative);

            line.loc = LOCCTR; // EQU does NOT change LOCCTR
        }

        // 不知道的opcode
        else if (!line.opcode.empty())
        {
            cout << "[ERROR] Invalid Opcode : "
                << line.opcode
                << " at line "
                << line.lineNumber
                << endl;
        }
    }

    programLength = LOCCTR - startAddress;

    cout << "\nPASS1 Finished\n";
    cout << "Start Address = " << hex << uppercase << startAddress << endl;
    cout << "Program Length = " << hex << uppercase << programLength << endl;
}

//output the symbol table
void Assembler::printSymtab()
{
    cout << "\n=SYMTAB=\n";

    for (auto& s : symtab)
    {
        cout << left << setw(8) << s.first
            << setw(8) << setfill(' ') << right
            << toHex(s.second.value, 4)
            << "        "
            << (s.second.relative ? "R" : "A")
            << "\n";
    }
}

void Assembler::writeSymtab(const string& filename)
{
    ofstream fout(filename);

    if (!fout)
    {
        cout << "[ERROR] Cannot open symbol file\n";
        return;
    }

    fout << "=SYMTAB=\n";

    for (auto& s : symtab)
    {
        fout << left << setw(8) << s.first
            << setw(8) << toHex(s.second.value, 4)
            << "    "
            << (s.second.relative ? "R" : "A")
            << "\n";
    }

    fout.close();

    cout << "[INFO] SYMTAB written to " << filename << endl;
}