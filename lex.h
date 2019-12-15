#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int stateTransitions[14][8] = 
           /* Start */                        {{1,  3,  12, 9,  9,  7,  0,  13},
           /* Process Alpha */                 {1,  1,  1,  12, 2,  12, 2,  12},
           /* Finish Alpha */                  {0,  0,  0,  0,  0,  0,  0,  0},
           /* Process Int */                   {12, 3,  12, 4,  5,  12, 5,  12},
           /* Process Float */                 {12, 4,  12, 12, 6,  12, 6,  12},
           /* Finish Int */                    {0,  0,  0,  0,  0,  0,  0,  0 },
           /* Finish Float */                  {0,  0,  0,  0,  0,  0,  0,  0 },
           /* Process Operator */              {12, 12, 12, 12, 12, 12, 8,  12},
           /* Finish Operator */               {0,  0,  0,  0,  0,  0,  0,  0 },
           /* Process Separator */             {12, 12, 12, 12, 12, 12, 11, 12},
           /* Finish Separator */              {0,  0,  0,  0,  0,  0,  0,  0 },
           /* Error */                         {0,  0,  0,  0,  0,  0,  0,  0 },
           /* End */                           {12, 12, 12, 12, 12, 12, 0,  12},
           /* Comment */                       {13, 13, 13, 13, 13, 13, 13, 0 }};

string currentStr = "";
size_t currentState = 0;
vector<string> keywords = {"int", "float", "bool", "if", "else", "then", "endif", 
                           "while", "whileend", "do", "doend", "for", "forend", 
                           "input", "output", "and", "or", "function"};
ofstream output;

size_t convertToIndex(char c)
{
    if (isalpha(c))
        return 0;
    if (isdigit(c))
        return 1;
    if (c == '$')
        return 2;
    if (c == '.')
        return 3;
    if (c == '\'' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' 
        || c == ']' || c == ',' || c == ':' || c == ';' || c == '.')
        return 4;
    if (c == '*' || c == '+' || c == '-' || c == '=' || c == '/' || c == '>' 
       || c == '<' || c == '>' || c == '%')
        return 5;
    if (c == ' ' || c == '\n' || c == '\t')
        return 6;
    if (c == '!')
        return 7;
    else
        return -1;
};

void processAlpha(char c)
{
    currentStr += c;
}

void finishSeparator(char c)
{
    output << "SEPARATOR          =             " << currentStr << endl;
    currentState = 0;
    currentStr = "";
}

void finishAlpha(char c)
{
    vector<string>::iterator it = find(keywords.begin(), keywords.end(), currentStr);
    if (it != keywords.end())
        output << "KEYWORD            =             " << currentStr << endl;
    else
        output << "IDENTIFIER         =             " << currentStr << endl;
    currentState = 0;
    currentStr = "";
    if (convertToIndex(c) == 4)
    {
        currentStr += c;
        finishSeparator(c);
    }
}

void processInt(char c)
{
    currentStr += c;
}

void processFloat(char c)
{
    currentStr += c;
}

void finishInt(char c)
{
    output << "INT_LITERAL        =             " << currentStr << endl;
    currentState = 0;
    currentStr = "";
    if (convertToIndex(c) == 4)
    {
        currentStr += c;
        finishSeparator(c);
    }
}

void finishFloat(char c)
{
    output << "FLOAT_LITERAL      =             " << currentStr << endl;
    currentState = 0;
    currentStr = "";
    if (convertToIndex(c) == 4)
    {
        currentStr += c;
        finishSeparator(c);
    }
}

void processOperator(char c)
{
    currentStr += c;
}

void finishOperator(char c)
{
    output << "OPERATOR           =             " << currentStr << endl;
    currentState = 0;
    currentStr = "";
}

void processSeparator(char c)
{
    currentStr = c;
    finishSeparator(c);
}

void error(char c)
{
    currentStr = "";
    cerr << "ENCOUNTERED ERROR IN STATE " << currentState 
         << " WHILE PROCESSING '" << c << "'" << endl;
}

void handleCurrentChar(char c)
{
    size_t ind = convertToIndex(c);
    switch(currentState)
    {
    case 0: break;
    case 1: processAlpha(c);
            break;
    case 2: finishAlpha(c);
            break;
    case 3: processInt(c);
            break;
    case 4: processFloat(c);
            break;
    case 5: finishInt(c);
            break;
    case 6: finishFloat(c);
            break;
    case 7: processOperator(c);
            break;
    case 8: finishOperator(c);
            break;
    case 9: processSeparator(c);
            break;
    case 10: finishSeparator(c);
             break;
    case 12: error(c);
             break;
    case 13: 
             break;
    default: cerr << "INVALID CASE. CHAR=" << c << "\n";
             break;
    }
}

void processFile(string fileName)
{
    output.open("tokens.txt");
    ofstream o;
    o.open(fileName, ios_base::app);
    o << " ";
    o.close();
    ifstream in;
    in.open(fileName);
    char c;
    while (in.get(c))
    {
        currentState = stateTransitions[currentState][convertToIndex(c)];

        handleCurrentChar(c);
    }
    in.close();
}
