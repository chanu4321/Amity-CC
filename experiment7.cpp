#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

// Global vector of tokens and current position index.
vector<string> tokens;
int pos = 0;

// Function to report a syntax error.
void error() {
    cout << "Syntax error at token: ";
    if (pos < tokens.size())
        cout << tokens[pos] << endl;
    else
        cout << "end of input" << endl;
    exit(1);
}

// Helper: if the current token matches the expected token, consume it.
void match(const string &expected) {
    if (pos < tokens.size() && tokens[pos] == expected)
        pos++;
    else
        error();
}

// Forward declarations for recursive descent functions.
void parseE();       // E → T E'
void parseEPrime();  // E' → + T E' | ε
void parseT();       // T → F T'
void parseTPrime();  // T' → * F T' | ε
void parseF();       // F → ( E ) | i

// E → T E'
void parseE() {
    parseT();
    parseEPrime();
}

// E' → + T E' | ε
void parseEPrime() {
    // While the next token is '+' continue the production.
    while (pos < tokens.size() && tokens[pos] == "+") {
        match("+");
        parseT();
    }
    // If the next token is not '+' then E' produces epsilon (do nothing).
}

// T → F T'
void parseT() {
    parseF();
    parseTPrime();
}

// T' → * F T' | ε
void parseTPrime() {
    // While the next token is '*' continue the production.
    while (pos < tokens.size() && tokens[pos] == "*") {
        match("*");
        parseF();
    }
    // Otherwise, T' produces epsilon.
}

// F → ( E ) | i
void parseF() {
    if (pos < tokens.size() && tokens[pos] == "(") {
        match("(");
        parseE();
        match(")");
    } else if (pos < tokens.size() && tokens[pos] == "i") {
        match("i");
    } else {
        error();
    }
}

int main() {
    cout << "Enter an expression (tokens separated by space, e.g.: i + i * i):\n";
    string input;
    getline(cin, input);

    // Tokenize the input string by spaces.
    istringstream iss(input);
    string token;
    while (iss >> token)
        tokens.push_back(token);

    // Begin parsing with the start symbol (E).
    parseE();

    // After parsing, all tokens should have been consumed.
    if (pos == tokens.size())
        cout << "Input parsed successfully." << endl;
    else {
        cout << "Error: input not fully consumed." << endl;
    }

    return 0;
}