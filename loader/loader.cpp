#include "loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

Loader::Loader()
{
    //memory.resize(1024 * 1024);

    startAddress = 0;
    programLength = 0;
    executionAddress = 0;
	loadAddress = 0;
}

void Loader::setLoadAddress(int addr)
{
    loadAddress = addr;
}

void Loader::load(const string& filename)
{
    ifstream fin(filename);

    if (!fin)
    {
        cout << "[ERROR] Cannot open file\n";
        return;
    }

    string line;

    while (getline(fin, line))
    {
        if (line.empty())
            continue;

        char type = line[0];

        // H RECORD
        if (type == 'H')
        {
            programName = line.substr(1, 6);

            // trim trailing spaces
            programName.erase(
                programName.find_last_not_of(' ') + 1
            );

            startAddress = stoi(line.substr(7, 6), nullptr, 16);
            programLength = stoi(line.substr(13, 6), nullptr, 16);

            cout << "[H] Name=" << programName
                << " Start=" << hex << startAddress
                << " Length=" << programLength << endl;
        }

        // T RECORD
        else if (type == 'T')
        {
            int objAddr = stoi(line.substr(1, 6), nullptr, 16);
            int len = stoi(line.substr(7, 2), nullptr, 16);
            string obj = line.substr(9);

            // relocation
            int realAddr = loadAddress + (objAddr - startAddress);

            cout << "[T] objAddr=" << hex << objAddr
                << " realAddr=" << realAddr
                << " len=" << len << endl;

            // write memory byte by byte
            for (int i = 0; i < obj.size(); i += 2)
            {
                string byteStr = obj.substr(i, 2);

                memory[realAddr] = (unsigned char)stoi(byteStr, nullptr, 16);

                realAddr++;
            }
        }

        // M RECORD
        else if (type == 'M')
        {
            int addr = stoi(line.substr(1, 6), nullptr, 16);
            int len = stoi(line.substr(7, 2), nullptr, 16);

            cout << "[M] Addr=" << hex << addr
                << " Len=" << len << endl;

            int realAddr = loadAddress + (addr - startAddress);

            int byteLen = (len + 1) / 2; // half-byte -> byte

            // 讀出原本的值
            int value = 0;

            for (int i = 0; i < byteLen; i++)
            {
                value = (value << 8) | memory[realAddr + i];
            }

            // 執行修改
            value += (loadAddress - startAddress);

            // 寫回 memory
            for (int i = byteLen - 1; i >= 0; i--)
            {
                memory[realAddr + i] = value & 0xFF;
                value >>= 8;
            }
        }

        // E RECORD
        else if (type == 'E')
        {
            executionAddress = stoi(line.substr(1, 6), nullptr, 16);

            cout << "[E] Entry=" << hex << executionAddress << endl;
        }
    }

    fin.close();
}

void Loader::printMemory()
{
    cout << "\n========== MEMORY DUMP ==========\n";

    if (memory.empty())
    {
        cout << "[EMPTY MEMORY]\n";
        return;
    }

    // 將 unordered_map 轉成 vector 以利排序
    vector<pair<int, unsigned char>> mem;

    for (auto& m : memory)
    {
        mem.push_back(m);
    }

    sort(mem.begin(), mem.end(),
        [](auto& a, auto& b)
        {
            return a.first < b.first;
        });

    // 每16個byte一行的方式輸出memory內容
    int lineStart = -1;
    int count = 0;

    for (size_t i = 0; i < mem.size(); i++)
    {
        int addr = mem[i].first;
        unsigned char val = mem[i].second;

        // new line start
        if (count == 0)
        {
            lineStart = addr;

            cout << hex << uppercase << setw(6) << setfill('0')
                << addr << " : ";
        }

        cout << setw(2) << setfill('0')
            << hex << (int)val << " ";

        count++;

        if (count == 16)
        {
            cout << "\n";
            count = 0;
        }
    }

    if (count != 0)
        cout << "\n";

    cout << "================================\n";
}