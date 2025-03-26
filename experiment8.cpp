#include <iostream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// Structure to hold a production rule.
struct Production {
    string lhs;            // Left-hand side (nonterminal)
    vector<string> rhs;    // Right-hand side (a sequence of symbols)
};

// tryReduce checks if the top of the stack (i.e. its last few symbols) matches
// the right-hand side of any production. If a match is found, it pops those symbols
// and pushes the left-hand side. It returns true if a reduction was performed.
bool tryReduce(vector<string>& stack, const vector<Production>& grammar) {
    // Try each production rule.
    for (const auto& prod : grammar) {
        int n = prod.rhs.size();
        if (stack.size() < n)
            continue;
        bool match = true;
        // Compare the last n symbols of the stack with prod.rhs.
        for (int i = 0; i < n; i++) {
            if (stack[stack.size() - n + i] != prod.rhs[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            // Print the reduction step.
            cout << "Reduce: ";
            for (int i = 0; i < n; i++) {
                cout << stack[stack.size() - n + i] << " ";
            }
            cout << "-> " << prod.lhs << endl;
            // Pop the matching symbols.
            for (int i = 0; i < n; i++) {
                stack.pop_back();
            }
            // Push the left-hand side (nonterminal) onto the stack.
            stack.push_back(prod.lhs);
            return true; // A reduction occurred; return immediately.
        }
    }
    return false; // No reduction was possible.
}

int main() {
    // Define the grammar.
    // S → C C
    // C → c C
    // C → d
    vector<Production> grammar;
    grammar.push_back({"S", {"C", "C"}});
    grammar.push_back({"C", {"c", "C"}});
    grammar.push_back({"C", {"d"}});

    // Read the input string (tokens separated by spaces).
    cout << "Enter input string (tokens separated by space, e.g.: d d or c d d): ";
    string input;
    getline(cin, input);
    
    // Tokenize the input.
    vector<string> tokens;
    istringstream iss(input);
    string tok;
    while (iss >> tok)
        tokens.push_back(tok);
    // Append the end marker.
    tokens.push_back("$");

    // Initialize the parsing stack with the end marker.
    vector<string> stack;
    stack.push_back("$");

    int index = 0;
    cout << "\nShift-Reduce Parsing Process:\n";
    while (true) {
        // Print current state.
        cout << "Stack: ";
        for (const auto &s : stack)
            cout << s << " ";
        cout << "\tInput: ";
        for (int i = index; i < tokens.size(); i++)
            cout << tokens[i] << " ";
        cout << endl;

        // Accept condition: when stack equals "$ S" and the input token is "$".
        if (stack.size() == 2 && stack[1] == "S" && stack[0] == "$"  && tokens[index] == "$") {
            cout << "\nInput accepted!\n";
            break;
        }

        // Attempt to reduce as long as possible.
        if (tryReduce(stack, grammar))
            continue;

        // If no reduction is possible, shift the next token.
        if (index < tokens.size()) {
            cout << "Shift: " << tokens[index] << endl;
            stack.push_back(tokens[index]);
            index++;
        } else {
            cout << "Parsing error: no more tokens to shift and no valid reduction.\n";
            break;
        }
    }

    return 0;
}