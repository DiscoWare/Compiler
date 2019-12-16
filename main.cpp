#include "lex.h"
#include "parse.h"
#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    ofstream assemblyOutput;
    assemblyOutput.open("output.txt");
    processFile("input.txt");
    tokenize();
    Driver();
    int instructionNumber = 1;
    for (auto x : instructionVec)
    {
        assemblyOutput << left << setw(10) << instructionNumber << setw(10) << x.Op_;
        if (x.Op_ != "ADD" && x.Op_ != "SUB" && x.Op_ != "MUL" && x.Op_ != "DIV")
            assemblyOutput << setw(10) << x.Oprnd_;
        assemblyOutput << endl;
        ++instructionNumber;
    }
    assemblyOutput << "\n\nSymbol Table\n" << left << setw(20) << "Identifier"          
                   << setw(20) << "Memory Location" << setw(20) << "Type" << endl;
    for (auto x : symbolVec)
        assemblyOutput << left << setw(20) << x.Identifier_ << setw(20) << x.MemoryLocation_ << setw(20) << x.Type_ << endl;
    output.close();
    return 0;
}