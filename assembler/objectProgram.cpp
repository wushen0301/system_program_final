#include "assembler.h"
#include "utils.h"

#include <iostream>
#include <fstream>

using namespace std;

void Assembler::generateObjectProgram()
{
    // H Record
    string programName = "NONAME";

    if (!sourceLines.empty())
    {
        if (!sourceLines[0].label.empty())
        {
            programName = sourceLines[0].label;
        }
    }

    headerRecord = "H" + programName + toHex(startAddress, 6) + toHex(programLength, 6);


    // T Record
    textRecords.clear();

    string currentData = "";
    int currentStart = -1;
    int currentLength = 0;

    for (auto& line : sourceLines)
    {
        // 沒有 object code
        if (line.objectCode.empty())
        {
            if (!currentData.empty())
            {
                string record = "T" + toHex(currentStart, 6) + toHex(currentLength, 2) + currentData;

                textRecords.push_back(record);

                currentData = "";
                currentStart = -1;
                currentLength = 0;
            }

            continue;
        }

        // 新的 T Record 起點
        if (currentStart == -1)
        {
            currentStart = line.loc;
        }

        // 目前 object code 幾 bytes
        int objBytes = static_cast<int>(line.objectCode.length()) / 2;

        // 超過 30 bytes，寫入新一行
        if (currentLength + objBytes > 30)
        {
            string record = "T" + toHex(currentStart, 6) + toHex(currentLength, 2) + currentData;

            textRecords.push_back(record);

            currentData = "";
            currentLength = 0;
            currentStart = line.loc;
        }

        // 加入目前 object code
        currentData += line.objectCode;
        currentLength += objBytes;
    }

    // M Record
    modificationRecords.clear();

    for (auto& line : sourceLines)
    {
        if (line.format != "format4")
            continue;

        string operand = line.operand;

        // 去掉 # 或 @
        if (!operand.empty() && (operand[0] == '#' || operand[0] == '@'))
        {
            operand = operand.substr(1);
        }

        // 去掉 ,X
        size_t pos = operand.find(",X");

        if (pos != string::npos)
        {
            operand = operand.substr(0, pos);
        }

        // 判斷是不是純數字
        bool isNumber = true;

        for (char c : operand)
        {
            if (!isdigit(c))
            {
                isNumber = false;
                break;
            }
        }

        // 使用立即值不用產生 M Record
        if (isNumber)
            continue;

        string record = "M" + toHex(line.loc + 1, 6) + "05+" + programName;
        modificationRecords.push_back(record);
    }

    // T Record
    if (!currentData.empty())
    {
        string record ="T" + toHex(currentStart, 6) + toHex(currentLength, 2) + currentData;
        textRecords.push_back(record);
    }

    // E Record
    endRecord = "E" + toHex(startAddress, 6);
}

void Assembler::printObjectProgram()
{
    cout << "\n===== OBJECT PROGRAM =====\n";

    cout << headerRecord << endl;

    for (auto& t : textRecords)
    {
        cout << t << endl;
    }

    for (auto& m : modificationRecords)
    {
        cout << m << endl;
    }
    cout << endRecord << endl;
}

void Assembler::writeObjectFile(const string& filename)
{
    ofstream fout(filename);

    if (!fout)
    {
        cout << "[ERROR] Cannot create " << filename << endl;
        return;
    }

    // H Record
    fout << headerRecord << '\n';

    // T Record
    for (auto& t : textRecords)
    {
        fout << t << '\n';
    }

    // M Record
    for (auto& m : modificationRecords)
    {
        fout << m << '\n';
    }

    // E Record
    fout << endRecord << '\n';

    fout.close();

    cout << "\nObject file generated : " << filename << endl;
}