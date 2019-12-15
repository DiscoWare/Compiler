#include "lex.h"
#include "parse.h"
#include <iostream>
using namespace std;

int main()
{
    processFile("input.txt");
    tokenize();
    Driver();
    return 0;
}