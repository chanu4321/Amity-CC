#include <iostream> 
#include <fstream> 
#include <sstream> 
#include <vector> 
#include <string> 
#include <unordered_map> 
#include <algorithm> 
using namespace std; 
// Global symbol table for variables. 
unordered_map<string, int> variables; 
// Splits a line into tokens (simple whitespace splitting). 
vector<string> tokenize(const string &line) { 
vector<string> tokens; 
istringstream iss(line); 
string token; 
while (iss >> token) { 
        tokens.push_back(token); 
    } 
    return tokens; 
} 
 
// Forward declaration. 
void executeLine(const string &line, int &i, const vector<string>& lines); 
 
// Executes a block (used in repeat constructs). 
void executeBlock(const vector<string>& blockLines) { 
    int j = 0; 
    while (j < blockLines.size()) { 
        executeLine(blockLines[j], j, blockLines); 
    } 
} 
 
// Executes a single line of code. 
void executeLine(const string &line, int &i, const vector<string>& lines) { 
    vector<string> tokens = tokenize(line); 
    if (tokens.empty()) { 
        i++; 
        return; 
    } 
    string command = tokens[0]; 
 
    if (command == "fun") { 
        // Do nothing. 
    } 
    else if (command == "declare") { 
        if (tokens.size() >= 2) { 
            string varName = tokens[1]; 
            variables[varName] = 0; 
        } 
    } 
    else if (command == "assign") { 
        if (tokens.size() >= 3) { 
            string varName = tokens[1]; 
            int value = stoi(tokens[2]); 
            variables[varName] = value; 
        } 
    } 
    else if (command == "compute") { 
        if (tokens.size() >= 4) { 
            string varName = tokens[1]; 
            string op = tokens[2]; 
            int operand = stoi(tokens[3]); 
            if (variables.find(varName) != variables.end()) { 
                if (op == "plus") 
                    variables[varName] += operand; 
                else if (op == "minus") 
                    variables[varName] -= operand; 
                else if (op == "multiply") 
                    variables[varName] *= operand; 
                else if (op == "divide") { 
                    if (operand != 0) 
                        variables[varName] /= operand; 
                } 
                else if (op == "mod") { 
                    if (operand != 0) 
                        variables[varName] %= operand; 
                } 
            } 
        } 
    } 
    else if (command == "display") { 
        if (tokens.size() >= 2) { 
            string varName = tokens[1]; 
            if (variables.find(varName) != variables.end()) 
                cout << variables[varName] << "\n"; 
        } 
    } 
    else if (command == "print") { 
        if (tokens.size() >= 2) { 
            // If token starts with a quote, treat as a string literal. 
            if (tokens[1][0] == '\"') { 
                string output; 
                bool first = true; 
                for (const string &t : tokens) { 
                    if (first) { 
                        output += t.substr(1); 
                        first = false; 
                    } else { 
                        output += " " + t; 
                    } 
                } 
                if (!output.empty() && output.back() == '\"') 
                    output.pop_back(); 
                cout << output << "\n"; 
            } else { 
                // Otherwise, assume it's a variable name. 
                string varName = tokens[1]; 
                if (variables.find(varName) != variables.end()) 
                    cout << variables[varName] << "\n"; 
            } 
        } 
    } 
    else if (command == "calc") { 
        // Do nothing. 
    } 
    else if (command == "if") { 
        if (tokens.size() >= 2) { 
            string varName = tokens[1]; 
            bool condition = false; 
            if (variables.find(varName) != variables.end()) 
                condition = (variables[varName] != 0); 
            i++; 
            if (i < lines.size()) { 
                if (condition) { 
                    executeLine(lines[i], i, lines); 
                    i++; 
                    if (i < lines.size() && tokenize(lines[i])[0] == "else") { 
                        i++; // Skip the else block. 
                    } 
                } else { 
                    i++; 
                    if (i < lines.size() && tokenize(lines[i])[0] == "else") { 
                        i++; 
                        if (i < lines.size()) { 
                            executeLine(lines[i], i, lines); 
                            i++; 
                        } 
                    } 
                } 
            } 
            return; 
        } 
    } 
    else if (command == "repeat") { 
        if (tokens.size() >= 2) { 
            int times = stoi(tokens[1]); 
            vector<string> blockLines; 
            i++; 
            while (i < lines.size() && lines[i] != "endrepeat") { 
                blockLines.push_back(lines[i]); 
                i++; 
            } 
            if (i < lines.size() && lines[i] == "endrepeat") 
                i++; 
            for (int t = 0; t < times; t++) { 
                executeBlock(blockLines); 
            } 
            return; 
        } 
    } 
    // For unknown commands, do nothing. 
    i++; 
} 
 
int main() { 
    ifstream inFile("input.txt"); 
    if (!inFile.is_open()) { 
        return 1; 
    } 
    vector<string> lines; 
    string line; 
    while (getline(inFile, line)) { 
        if (!line.empty()) 
            lines.push_back(line); 
    } 
    inFile.close(); 
 
    int i = 0; 
    while (i < lines.size()) { 
        executeLine(lines[i], i, lines); } 
return 0;} 