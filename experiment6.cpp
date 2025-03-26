#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <vector>
using namespace std;

int main() {
    // Define the predictive parsing table for the grammar.
    // Nonterminals: E, E', T, T', F
    // Terminals: i, +, *, (, ), and end marker $
    unordered_map<string, unordered_map<string, string>> parseTable;
    
    // Fill table for nonterminal E
    parseTable["E"]["i"] = "T E'";
    parseTable["E"]["("] = "T E'";
    
    // Fill table for nonterminal E'
    parseTable["E'"]["+"] = "+ T E'";
    parseTable["E'"][")"] = "ε";
    parseTable["E'"]["$"] = "ε";
    
    // Fill table for nonterminal T
    parseTable["T"]["i"] = "F T'";
    parseTable["T"]["("] = "F T'";
    
    // Fill table for nonterminal T'
    parseTable["T'"]["+"] = "ε";
    parseTable["T'"]["*"] = "* F T'";
    parseTable["T'"][")"] = "ε";
    parseTable["T'"]["$"] = "ε";
    
    // Fill table for nonterminal F
    parseTable["F"]["i"] = "i";
    parseTable["F"]["("] = "( E )";
    
    // Read input string (tokens separated by space)
    string input;
    cout << "Enter the input string (tokens separated by space, end with $): ";
    getline(cin, input);
    
    // Tokenize the input string
    vector<string> tokens;
    istringstream iss(input);
    string tok;
    while (iss >> tok)
        tokens.push_back(tok);
    
    // Ensure the input ends with the end marker "$"
    if (tokens.empty() || tokens.back() != "$")
        tokens.push_back("$");
    
    // Create a stack and push the start symbol and end marker.
    stack<string> parseStack;
    parseStack.push("$");
    parseStack.push("E");  // Start symbol is E
    
    int index = 0; // pointer to the current input token
    
    cout << "\nParsing Process:\n";
    while (!parseStack.empty()) {
        string top = parseStack.top();
        string currentToken = tokens[index];
        cout << "Stack top: " << top << "    Current token: " << currentToken << "\n";
        
        // If both the top and input are the end marker, parsing is successful.
        if (top == "$" && currentToken == "$") {
            cout << "\nParsing Successful!\n";
            break;
        }
        
        // If the top of the stack is a terminal
        if (top == currentToken) {
            parseStack.pop();
            index++;
        }
        // Otherwise, if the top is a nonterminal, look it up in the parsing table.
        else if (parseTable.find(top) != parseTable.end()) {
            if (parseTable[top].find(currentToken) != parseTable[top].end()) {
                string production = parseTable[top][currentToken];
                cout << top << " -> " << production << "\n";
                parseStack.pop();
                // If production is not epsilon, xpush its symbols in reverse order.
                if (production != "ε") {
                    // Tokenize the production (symbols are separated by spaces)
                    vector<string> prodSymbols;
                    istringstream prodStream(production);
                    string symbol;
                    while (prodStream >> symbol)
                        prodSymbols.push_back(symbol);
                    for (auto it = prodSymbols.rbegin(); it != prodSymbols.rend(); it++)
                        parseStack.push(*it);
                }
            } else {
                cout << "Error: No rule for (" << top << ", " << currentToken << ")\n";
                break;
            }
        }
        // If top is a terminal but does not match current token, it's an error.
        else {
            cout << "Error: Terminal mismatch, expected " << top << " but found " << currentToken << "\n";
            break;
        }
    }
    
    if (parseStack.empty() && index == tokens.size())
        cout << "\nParsing Complete.\n";
    else if (!parseStack.empty())
        cout << "\nParsing Failed.\n";
    
    return 0;
}