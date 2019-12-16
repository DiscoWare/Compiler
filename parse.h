#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

////////////////////////////////
// Function Definitions
////////////////////////////////
void Driver();

/////////////////////////////////////
// Token struct for Token and Lexeme
/////////////////////////////////////
struct Token
{
    Token(const string& token, const string& lexeme) { token_ = token; lexeme_ = lexeme; };
    Token() {};

    string token_;
    string lexeme_;
};

///////////////////////////////////////
// Instruction struct 
///////////////////////////////////////
struct Instruction
{
    Instruction(const string& Op, const int Oprnd) { Op_ = Op; Oprnd_ = Oprnd; }
    Instruction() { Oprnd_ = 0; }

    string Op_;
    int Oprnd_;
};

///////////////////////////////////////
// Symbol Struct
///////////////////////////////////////
struct Symbol
{
    Symbol(const string& Identifier, int MemoryLocation, const string& Type) 
        { Identifier_ = Identifier; MemoryLocation_ = MemoryLocation; Type_ = Type; }
    Symbol()
        { MemoryLocation_ = 0; }

    string Identifier_;
    int MemoryLocation_;
    string Type_;
};

////////////////////////////////
// Global Variables
////////////////////////////////
string workingString;
ofstream parseOutput;
ofstream assemblyOutput;
stack<char> Stack;
vector<Token> tokenVec;
vector<Token>::const_iterator currentToken;
vector<Instruction> instructionVec;
vector<Symbol> symbolVec;
int memoryLocation = 2000;
int assignmentMemLocation = -1;
Symbol* currentSymbol;

////////////////////////////////
// Production Table
////////////////////////////////
string Table[8][9] = {{"A;", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "D;", "C"},
                      {"i=E", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID"},
                      {"iF", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID"},
                      {"INVALID", "+i", "-i", "*i", "INVALID", "INVALID", "EPSILON", "INVALID", "INVALID"},
                      {"SU", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "SU", "SU"},
                      {"TU", "INVALID", "INVALID", "INVALID", "INVALID", "EPSILON", "EPSILON", "TU", "TU"},
                      {"INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "EPSILON", "INVALID", "ti=E", "INVALID"},
                      {"INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "k(ici)"}};

//////////////////////////////////
// Find Symbol
//////////////////////////////////
bool findSymbol(const string& symbolID)
{
    vector<Symbol>::const_iterator iter = symbolVec.cbegin();
    while (iter != symbolVec.cend())
    {
        if (iter->Identifier_ == symbolID)
            return true;
        iter++;
    }
    return false;
} 

///////////////////////////////////
// Assignment Function
///////////////////////////////////
void assemblyAssignment()
{
    vector<Token>::const_iterator assemblyCurrentToken = currentToken;
    if (assemblyCurrentToken->lexeme_ == "int")
    {
        assemblyCurrentToken++;
        if (findSymbol(assemblyCurrentToken->lexeme_))
            cerr << "ERROR. Initialized variable redefined\n";
    }
    if (!findSymbol(assemblyCurrentToken->lexeme_))
    {
        Symbol newSymbol(assemblyCurrentToken->lexeme_, memoryLocation, "int");
        assignmentMemLocation = memoryLocation;
        memoryLocation++;
        symbolVec.push_back(newSymbol);
        currentSymbol = &symbolVec[symbolVec.size() - 1];
    }
    else
    {
        for (auto x : symbolVec)
        {
            if (x.Identifier_ == assemblyCurrentToken->lexeme_)
                assignmentMemLocation = x.MemoryLocation_;
        }
    }
    
};

//////////////////////////////////////////////////////////////
// Get memory address of variables already declared variables
//////////////////////////////////////////////////////////////
int getAddress(const string& sym)
{
    vector<Symbol>::const_iterator x = symbolVec.cbegin();
    while (x != symbolVec.cend())
    {
        if (x->Identifier_ == sym)
            return x->MemoryLocation_;
        x++;
    }
    cerr << "ERROR: " << sym << " NOT A VALID VARIABLE NAME\n";
    return -1;
}

/////////////////////////////////////////////////////////
// Expression function
/////////////////////////////////////////////////////////
void assemblyExpression()
{
    Instruction newInstruction;
    vector<Token>::const_iterator assemblyCurrentToken = currentToken;

    assemblyCurrentToken++;
    bool complexExpression = (assemblyCurrentToken->lexeme_ != ";");
    assemblyCurrentToken--;
    
    if (!complexExpression)
    {
        newInstruction.Op_ = "PUSHI";
        newInstruction.Oprnd_ = stoi(assemblyCurrentToken->lexeme_);
        assemblyCurrentToken++;
        instructionVec.push_back(newInstruction);
        newInstruction.Op_ = "POPM";
        newInstruction.Oprnd_ = memoryLocation - 1;
        instructionVec.push_back(newInstruction);
    }
    else
    {
        newInstruction.Op_ = "PUSHM";
        newInstruction.Oprnd_ = getAddress(assemblyCurrentToken->lexeme_);
        instructionVec.push_back(newInstruction);
        assemblyCurrentToken++;
        string oper = assemblyCurrentToken->lexeme_;
        assemblyCurrentToken++;
        newInstruction.Oprnd_ = getAddress(assemblyCurrentToken->lexeme_);
        instructionVec.push_back(newInstruction);
        if (oper == "+")
        {
            newInstruction.Op_ = "ADD";
            instructionVec.push_back(newInstruction);
        }
        else if (oper == "-")
        {
            newInstruction.Op_ = "SUB";
            instructionVec.push_back(newInstruction);
        }
        else if (oper == "*")
        {
            newInstruction.Op_ = "MUL";
            instructionVec.push_back(newInstruction);
        }
        else if (oper == "/")
        {
            newInstruction.Op_ = "DIV";
            instructionVec.push_back(newInstruction);
        }
        newInstruction.Op_ = "POPM";
        newInstruction.Oprnd_ = assignmentMemLocation;
        instructionVec.push_back(newInstruction);
    }
    
}

/////////////////////////////////////////
// Convert To Index for Production Table
/////////////////////////////////////////
int convertToIndex(Token token)
{
    if (token.token_ == "IDENTIFIER" || token.token_ == "INT_LITERAL")
    {
        return 0;
    }
    else if (token.lexeme_ == "+")
    {
        return 1;
    }
    else if (token.lexeme_ == "-")
    {
        return 2;
    }
    else if (token.lexeme_ == "*")
    {
        return 3;
    }
    else if (token.lexeme_ == "=")
    {
        return 4;
    }
    else if (token.lexeme_ == "$")
    {
        return 5;
    }
    else if (token.lexeme_ == ";")
    {
        return 6;
    }
    else if (token.lexeme_ == "int")
    {
        return 7;
    }
    else if (token.lexeme_ == "if" || token.lexeme_ == "while")
    {
        return 8;
    }
    return -1;
};

//////////////////////////////////////////////
// Convert Non Terminals for Production Table
//////////////////////////////////////////////
int convertNonTerminal(char c)
{
    if (c == 'S')
        return 0;
    else if (c == 'A')
        return 1;
    else if (c == 'E')
        return 2;
    else if (c == 'F')
        return 3;
    else if (c == 'T')
        return 4;
    else if (c == 'U')
        return 5;
    else if (c == 'D')
        return 6;
    else if (c == 'C')
        return 7;
    else
        return -1;
}

//////////////////////////////////////////
// Optional print function for debugging
//////////////////////////////////////////
void printStack()
{
    stack<char> newStack = Stack;
    while (!Stack.empty())
    {
        parseOutput << "| " << Stack.top() << " |" << endl;
        Stack.pop();
    }
    Stack = newStack;

}

//////////////////////////////////////////////////
// Put all tokens from source code into .txt file
//////////////////////////////////////////////////
void tokenize()
{
    ifstream in;
    in.open("tokens.txt");
    string str;
    Token token;
    while (in >> str)
    {
        token.token_ = str;
        in >> str;
        in >> str;
        token.lexeme_ = str;
        tokenVec.push_back(token);
    }
    in.close();
}

////////////////////////////////
// For pushing in reverse order
////////////////////////////////
void reversePush(const string& str)
{
    if (str == "EPSILON")
        return;
    for (int i = str.size() - 1; i >= 0; i--)
    {
        Stack.push(str[i]);
    }
}

///////////////////////////////////////////
// Optional Stack Dump for Error Debugging
///////////////////////////////////////////
void dumpStack()
{
    parseOutput << "Contents of Stack:\n";
    while (!Stack.empty())
    {
        parseOutput << Stack.top() << endl;
        Stack.pop();
    }
}

////////////////////////////////
// Handling Terminals
////////////////////////////////
void handleTerminal(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_.size() == 1 && currentToken->lexeme_[0] == Stack.top())
    {
        Stack.pop();
        parseOutput << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        currentToken++;
    }
    else
    {
        parseOutput << "ERROR: EXPECTED " << Stack.top() << " BUT GOT " << currentToken->lexeme_ << endl;
    }
}

/////////////////////////////////
// Handling Identifiers/Literals
/////////////////////////////////
void handleIdentifier(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->token_ == "IDENTIFIER" || currentToken->token_ == "INT_LITERAL")
    {
        parseOutput << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
    }
    else
        parseOutput << "ERROR: EXPECTED IDENTIFIER OR LITERAL BUT RECEIVED: " << currentToken->lexeme_ << endl;
        
    Stack.pop();
    currentToken++;
}

////////////////////////////////
// Handle Compare Statements
////////////////////////////////
void handleCompare(vector<Token>::const_iterator& currentToken)
{
    if (Stack.top() == 'c')
    {
        if (currentToken->lexeme_ == ">" || currentToken->lexeme_ == "<" || currentToken->lexeme_ == "==" )
            parseOutput << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        else
            parseOutput << "ERROR: EXPECTED COMPARE OPERATOR BUT RECEIVED: " << currentToken->lexeme_ << endl;
    }
    else if (Stack.top() == 'k')
    {
        if (currentToken->lexeme_ == "if" || currentToken->lexeme_ == "while")
            parseOutput << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        else
            parseOutput << "ERROR: EXPECTED COMPARE KEYWORD BUT RECEIVED: " << currentToken->lexeme_ << endl;
    }
    else 
        parseOutput << "ERROR. EXPECTED c or k ON THE STACK\n";
    Stack.pop();
    currentToken++;

}

////////////////////////////////
// Handle Type Declarations
////////////////////////////////
void handleType(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_ == "int" || currentToken->lexeme_ == "float")
        parseOutput << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
    else
        parseOutput << "ERROR: EXPECTED VARIABLE TYPE BUT RECEIVED: " << currentToken->lexeme_ << endl;
    Stack.pop();
    currentToken++;
}

////////////////////////////////
// Finish File
////////////////////////////////
void finishFile(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_ == "$")
        parseOutput << "FILE PARSED SUCCESSFULLY\n";
    else
        parseOutput << "PARSER FAILED. EOF SYMBOL NOT ON TOP OF STACK\n";
    Stack.pop();
}

////////////////////////////////////
// Information for Production Rules
////////////////////////////////////
void printNonTerminalInfo()
{
    string message;
    switch (Stack.top())
    {
        case 'S': 
            message = "<Statement> -> <Assignment> | <Declaration> | <Compare>";
            assemblyAssignment();
            break;
        case 'A': 
            message = "<Assignment> -> id = <Expression>";
            break;
        case 'E': 
            message = "<Expression> -> id <Expression Prime>";
            assemblyExpression();
            break;
        case 'F': 
            message = "<Expression Prime> -> +id | -id | *id | epsilon";
            break;
        case 'T': 
            message = "<Statements> -> <Statement> <Statements Prime>";
            break;
        case 'U': 
            message = "<Statements Prime> -> <Statements> <Statements Prime> | epsilon";
            break;
        case 'D': 
            message = "<Declarative> -> type id = <Expression>";
            break;
        case 'C': 
            message = "<Compare Statement> -> compare_keyword ( id compare_operator id )";
            break;
        default:
            parseOutput << "ERROR: NON-TERMINAL ON TOP OF STACK: " << Stack.top();
    }
    parseOutput << "     " << message << endl;
}

//////////////////////////////////////////////////////////
// Handle Non-Terminals; Check Stack Equals Current Token
//////////////////////////////////////////////////////////
void handleNonTerminal(vector<Token>::const_iterator& currentToken)
{
    printNonTerminalInfo();
    workingString = Table[convertNonTerminal(Stack.top())][convertToIndex(*currentToken)];
    if (workingString != "INVALID")
    {
        Stack.pop();
        if (workingString == "EPSILON")
        {
            return;
        }
        reversePush(workingString);
    }
    else
    {
        parseOutput << "ERROR. GOT INVALID TABLE VALUE WHILE WORKING ON LEXEME: " << currentToken->lexeme_ 
             << ". Top of Stack: " << Stack.top() << endl;
        dumpStack();
    }
}

////////////////////////////////
// Main Driver Function
////////////////////////////////
void Driver()
{
    // Driver setup
    parseOutput.open("parseOutput.txt");
    Stack.push('$');
    Stack.push('T');
    tokenVec.push_back(Token("SEPARATOR", "$"));
    currentToken = tokenVec.begin();

    // Main loop--Only stops once the stack is empty
    while (!Stack.empty())
    {
        // printStack();                  // Optional stack printing for debugging
        if (Stack.top() == '+' || Stack.top() == '*' || Stack.top() == '=' || Stack.top() == '-' 
                   || Stack.top() == ';' || Stack.top() == ')' || Stack.top() == '(')
            handleTerminal(currentToken);
        else if (Stack.top() == 'i')
            handleIdentifier(currentToken);
        else if (Stack.top() == 'c' || Stack.top() == 'k')
            handleCompare(currentToken);
        else if (Stack.top() == '$')
            finishFile(currentToken);
        else if (Stack.top() == 't')
            handleType(currentToken);
        else
            handleNonTerminal(currentToken); 
    }
    output.close();
}