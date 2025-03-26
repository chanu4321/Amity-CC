#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
using namespace std;

struct Production {
    string lhs;
    vector<string> rhs;
};

int main() {
    // Define the grammar:
    // E -> T
    // E -> T + E
    // T -> F
    // T -> F * T
    // F -> id
    // F -> ( E )
    vector<Production> grammar = {
        {"E", {"T"}},
        {"E", {"T", "+", "E"}},
        {"T", {"F"}},
        {"T", {"F", "*", "T"}},
        {"F", {"id"}},
        {"F", {"(", "E", ")"}}
    };

    // Define nonterminals (as given)
    unordered_set<string> nonterminals = {"E", "T", "F"};
    
    // Collect all terminals (any symbol in an RHS that is not a nonterminal)
    unordered_set<string> terminals;
    for (auto &prod : grammar) {
        for (auto &sym : prod.rhs) {
            if (nonterminals.find(sym) == nonterminals.end())
                terminals.insert(sym);
        }
    }
    terminals.insert("$"); // add end-marker

    // -------------------------------
    // Compute Leading sets for each nonterminal.
    // Leading(X): set of terminals that can appear first in some string derived from X.
    unordered_map<string, unordered_set<string>> Leading;
    for (auto nt : nonterminals)
        Leading[nt] = unordered_set<string>();

    // Initialization: for each production A -> α
    for (auto &prod : grammar) {
        if (!prod.rhs.empty()) {
            string first = prod.rhs[0];
            // If first symbol is terminal, add it.
            if (nonterminals.find(first) == nonterminals.end())
                Leading[prod.lhs].insert(first);
            // If first symbol is nonterminal and there is a second symbol that is terminal, add it.
            if (prod.rhs.size() >= 2 && nonterminals.find(first) != nonterminals.end()) {
                string second = prod.rhs[1];
                if (nonterminals.find(second) == nonterminals.end())
                    Leading[prod.lhs].insert(second);
            }
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &prod : grammar) {
            if (!prod.rhs.empty()) {
                string first = prod.rhs[0];
                if (nonterminals.find(first) != nonterminals.end()) {
                    // Add Leading(first) to Leading(prod.lhs)
                    for (auto t : Leading[first]) {
                        if (Leading[prod.lhs].find(t) == Leading[prod.lhs].end()) {
                            Leading[prod.lhs].insert(t);
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // -------------------------------
    // Compute Trailing sets for each nonterminal.
    // Trailing(X): set of terminals that can appear last in some string derived from X.
    unordered_map<string, unordered_set<string>> Trailing;
    for (auto nt : nonterminals)
        Trailing[nt] = unordered_set<string>();

    // Initialization: for each production A -> α, consider the last symbol.
    for (auto &prod : grammar) {
        if (!prod.rhs.empty()) {
            string last = prod.rhs.back();
            if (nonterminals.find(last) == nonterminals.end())
                Trailing[prod.lhs].insert(last);
            // If the last symbol is nonterminal and there is a symbol before it which is terminal, add that terminal.
            if (prod.rhs.size() >= 2 && nonterminals.find(last) != nonterminals.end()) {
                string secondLast = prod.rhs[prod.rhs.size() - 2];
                if (nonterminals.find(secondLast) == nonterminals.end())
                    Trailing[prod.lhs].insert(secondLast);
            }
        }
    }
    changed = true;
    while (changed) {
        changed = false;
        for (auto &prod : grammar) {
            if (!prod.rhs.empty()) {
                string last = prod.rhs.back();
                if (nonterminals.find(last) != nonterminals.end()) {
                    for (auto t : Trailing[last]) {
                        if (Trailing[prod.lhs].find(t) == Trailing[prod.lhs].end()) {
                            Trailing[prod.lhs].insert(t);
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // -------------------------------
    // Build the operator precedence table.
    // The table is a mapping from terminal a to terminal b to a relation:
    // '<', '=', '>', or a blank if no relation exists.
    unordered_map<string, unordered_map<string, char>> precedence;
    // Initialize table entries to blank for every pair of terminals.
    for (auto a : terminals) {
        for (auto b : terminals)
            precedence[a][b] = ' ';
    }

    // For each production, examine adjacent symbols.
    for (auto &prod : grammar) {
        for (size_t i = 0; i < prod.rhs.size() - 1; i++) {
            string X = prod.rhs[i];
            string Y = prod.rhs[i+1];
            // Case 1: If X and Y are both terminals, then X = Y.
            if (nonterminals.find(X) == nonterminals.end() && nonterminals.find(Y) == nonterminals.end())
                precedence[X][Y] = '=';
            // Case 2: If X is terminal and Y is nonterminal, then for each terminal t in Leading(Y), add X < t.
            if (nonterminals.find(X) == nonterminals.end() && nonterminals.find(Y) != nonterminals.end()) {
                for (auto t : Leading[Y])
                    precedence[X][t] = '<';
            }
            // Case 3: If X is nonterminal and Y is terminal, then for each terminal t in Trailing(X), add t > Y.
            if (nonterminals.find(X) != nonterminals.end() && nonterminals.find(Y) == nonterminals.end()) {
                for (auto t : Trailing[X])
                    precedence[t][Y] = '>';
            }
            // Case 4: If there is a triple: X, Y, Z with X terminal, Y nonterminal, and Z terminal, then add X = Z.
            if (i + 2 < prod.rhs.size()) {
                string Z = prod.rhs[i+2];
                if (nonterminals.find(X) == nonterminals.end() && nonterminals.find(Y) != nonterminals.end() && nonterminals.find(Z) == nonterminals.end())
                    precedence[X][Z] = '=';
            }
        }
    }

    // Add relations with the boundary marker $.
    for (auto a : terminals) {
        if (a != "$") {
            precedence["$"][a] = '<';
            precedence[a]["$"] = '>';
        }
    }
    precedence["$"]["$"] = ' ';

    // -------------------------------
    // Print Leading and Trailing sets.
    cout << "Leading sets:\n";
    for (auto nt : nonterminals) {
        cout << nt << " : { ";
        for (auto t : Leading[nt])
            cout << t << " ";
        cout << "}\n";
    }
    cout << "\nTrailing sets:\n";
    for (auto nt : nonterminals) {
        cout << nt << " : { ";
        for (auto t : Trailing[nt])
            cout << t << " ";
        cout << "}\n";
    }

    // -------------------------------
    // Print the operator precedence parsing table.
    cout << "\nOperator Precedence Parsing Table:\n";
    
    // Print header row.
    cout << setw(8) << "";
    for (auto b : terminals)
        cout << setw(8) << b;
    cout << "\n";
    
    // Print each row.
    for (auto a : terminals) {
        cout << setw(8) << a;
        for (auto b : terminals)
            cout << setw(8) << precedence[a][b];
        cout << "\n";
    }
    
    return 0;
}
