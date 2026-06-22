#include <iostream>
#include "loader.h"

using namespace std;

int main()
{
    cout << "=========================\n";
    cout << "  SIC/XE LOADER START\n";
    cout << "=========================\n\n";

    Loader loader;
    int addr;

    // 因為只是模擬，因此loading address使用輸入的方式決定
    cout << "Enter load address (hex): ";
    cin >> hex >> addr;
    loader.setLoadAddress(addr);

    cout << "[1] Loading object program...\n";
    loader.load("output.obj");

    cout << "\n[2] Display memory content...\n";
    loader.printMemory();

    cout << "\n=========================\n";
    cout << "       FINISHED\n";
    cout << "=========================\n";

    return 0;
}