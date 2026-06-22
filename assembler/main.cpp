#include "assembler.h"

#include <iostream>

using namespace std;

int main()
{
    Assembler assembler;

    assembler.readSource("FIN-codetest 2026.txt");

    assembler.pass1();
    assembler.writeSymtab("symbol.txt");
	assembler.pass2();

    assembler.generateObjectProgram();
    //assembler.printObjectProgram();
    //assembler.writeObjectFile("output.obj");

    assembler.printSymtab();
    cout << endl;
	assembler.printPass2();
    //assembler.printSourceLines();


    return 0;
}