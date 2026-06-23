#include "assembler.h"
#include "optab.h"
#include "utils.h"

#include <iostream>
#include <sstream>

using namespace std;

void Assembler::pass2()
{
    std::cout << "\nPASS2 START\n";

    for (auto& line : sourceLines)
    {
        if (line.comment)
            continue;

        line.objectCode = "";
        line.format = "";
        line.mode = "";
        line.addrModeName = "";
        line.nixbpe = "";

        auto it = OPTAB.find(line.opcode);  //如果是format 4會找不到符合的

        // 判斷是否為format 4
        bool isFormat4 = false;
        string opcodeStr = line.opcode;

        if (!opcodeStr.empty() && opcodeStr[0] == '+')
        {
            isFormat4 = true;
            opcodeStr = opcodeStr.substr(1);
        }

        it = OPTAB.find(opcodeStr);

        if (it == OPTAB.end())
        {
            // WORD
            if (line.opcode == "WORD")
            {
                int value = stoi(line.operand);

                line.format = "WORD";
                line.objectCode = toHex(value, 6);

                continue;
            }

            // BYTE
            else if (line.opcode == "BYTE")
            {
                line.format = "BYTE";

                string obj;

                if (line.operand[0] == 'X')
                {
                    obj = line.operand.substr(
                        2,
                        line.operand.size() - 3
                    );
                }
                else if (line.operand[0] == 'C')
                {
                    string chars =
                        line.operand.substr(
                            2,
                            line.operand.size() - 3
                        );

                    for (char c : chars)
                    {
                        obj += toHex((unsigned char)c, 2);
                    }
                }

                line.objectCode = obj;

                continue;
            }

            // RESW / RESB
            else if (
                line.opcode == "RESW" ||
                line.opcode == "RESB")
            {
                line.format = line.opcode;
                line.objectCode = "";

                continue;
            }

            continue;
        }

        int format = it->second.format;
        int opcode = it->second.opcode;

        // RSUB special case
        if (line.opcode == "RSUB")
        {
            int obj =
                (opcode << 16) |
                (3 << 16);

            line.objectCode = toHex(obj, 6);

            line.format = "format3";

            line.mode = "Simple";
            line.addrModeName = "pc-relative";
            line.nixbpe = "110000";

            continue;
        }

        format = isFormat4 ? 4 : format;
        line.format = "format" + to_string(format);

        // format 2
        if (format == 2)
        {
            line.mode = "Simple";
            line.addrModeName = "register";

            int r1 = 0, r2 = 0;

            stringstream ss(line.operand);
            string reg1, reg2;

            getline(ss, reg1, ',');

            if (getline(ss, reg2, ','))
                ;

            if (REGTAB.count(reg1))
                r1 = REGTAB[reg1];

            if (REGTAB.count(reg2))
                r2 = REGTAB[reg2];

            int obj = (opcode << 8) | (r1 << 4) | r2;

            line.objectCode = toHex(obj, 4);

            continue;
        }

        //format 4
        else if (format == 4)
        {
            string operand = line.operand;

            bool n = true, i = true, x = false, b = false, p = false, e = true;

            // Immediate
            if (!operand.empty() && operand[0] == '#')
            {
                n = false;
                i = true;

                line.mode = "Immediate";

                operand = operand.substr(1);
            }

            // Indirect
            else if (!operand.empty() && operand[0] == '@')
            {
                n = true;
                i = false;

                line.mode = "Indirect";

                operand = operand.substr(1);
            }

            else
            {
                line.mode = "Simple";
            }

            // Indexed
            if (operand.find(",X") != string::npos)
            {
                x = true;

                operand = operand.substr(0, operand.find(",X"));
            }

            int addr = 0;

            // immediate constant
            if (!operand.empty() && isdigit(operand[0]))
            {
                addr = stoi(operand);
            }
            else if (symtab.count(operand))
            {
                addr = symtab[operand].value;
            }

            line.addrModeName = "absolute";

            int nixbpe =
                (n << 5) |
                (i << 4) |
                (x << 3) |
                (b << 2) |
                (p << 1) |
                e;

            line.nixbpe =
                to_string(n) +
                to_string(i) +
                to_string(x) +
                to_string(b) +
                to_string(p) +
                to_string(e);

            int obj =
                (opcode << 24) |
                (nixbpe << 20) |
                (addr & 0xFFFFF);

            line.objectCode = toHex(obj, 8);

            continue;
        }

        // format 3
        string operand = line.operand;

        bool n = true, i = true, x = false, b = false, p = true, e = false;

        // addressing mode
        bool immediateConstant = false;

        if (!operand.empty() && operand[0] == '#')
        {
            n = false;
            i = true;
            line.mode = "Immediate";
            operand = operand.substr(1);

            if (!operand.empty() && isdigit(operand[0]))
            {
                immediateConstant = true;
            }
        }
        else if (!operand.empty() && operand[0] == '@')
        {
            n = true;
            i = false;
            line.mode = "Indirect";
            operand = operand.substr(1);
        }
        else
        {
            line.mode = "Simple";
        }

        // indexed addressing
        if (operand.find(",X") != string::npos)
        {
            x = true;
            operand = operand.substr(0, operand.find(",X"));
        }

		// BASE directive
        if (line.opcode == "BASE")
        {
            continue;
        }

        // get address
        int addr = 0;

        if (immediateConstant)
        {
            addr = stoi(operand);
        }
        else
        {
            if (symtab.count(operand))
                addr = symtab[operand].value;
        }

        //處理立即值的特例
        if (immediateConstant)
        {
            b = 0;
            p = 0;

            int xbpe =
                (n << 5) |
                (i << 4) |
                (x << 3) |
                (b << 2) |
                (p << 1) |
                e;

            line.nixbpe =
                to_string(n) +
                to_string(i) +
                to_string(x) +
                to_string(b) +
                to_string(p) +
                to_string(e);

            line.addrModeName = "immediate";

            int obj =
                (opcode << 16) |
                (xbpe << 12) |
                (addr & 0xFFF);

            line.objectCode =
                toHex(obj, 6);

            continue;
        }

        // PC-relative
        int pc = line.loc + 3;
        int pcDisp = addr - pc;

        line.addrModeName = "pc-relative";

		// if PC-relative displacement is out of range, use base-relative
        bool useBase = false;

        if (pcDisp < -2048 || pcDisp > 2047)
            useBase = true;

        int disp = pcDisp;

        if (useBase)
        {
            if (!baseSet)
            {
                std::cout << "[ERROR] No BASE set\n";
            }

            if (disp < 0 || disp > 4095)
            {
                cout << "[ERROR] Cannot resolve with PC or BASE at line " << dec << line.lineNumber << endl;
            }

            disp = addr - baseRegister;

            line.addrModeName = "base-relative";
            b = 1;
            p = 0;
        }
        else
        {
            line.addrModeName = "pc-relative";
            b = 0;
            p = 1;
        }

        // nixbpe
        int xbpe =
            (n << 5) |
            (i << 4) |
            (x << 3) |
            (b << 2) |
            (p << 1) |
            e;

        line.nixbpe =
            to_string(n) +
            to_string(i) +
            to_string(x) +
            to_string(b) +
            to_string(p) +
            to_string(e);

        int obj = (opcode << 16) | (xbpe << 12) | (disp & 0xFFF);
        line.objectCode = toHex(obj, 6);
    }

    std::cout << "PASS2 DONE\n";
}