#include <bits/stdc++.h>
using namespace std;

// Helper function to trim whitespace from both ends of a string
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

void removeLeftRecursion(const string &nonTerminal, vector<string> &productions) {
    vector<string> alpha, beta;
    
    // Separate productions into left-recursive (alpha) and non-left-recursive (beta)
    for (const string &prod : productions) {
        string p = trim(prod);
        // Check if production begins with the entire nonTerminal
        if (p.substr(0, nonTerminal.size()) == nonTerminal) {
            string remainder = trim(p.substr(nonTerminal.size()));
            alpha.push_back(remainder);
        } else {
            beta.push_back(p);
        }
    }
    
    // If no left recursion is found, output the rule as is
    if (alpha.empty()) {
        cout << nonTerminal << " -> ";
        for (size_t i = 0; i < productions.size(); i++) {
            if (i > 0)
                cout << " | ";
            cout << trim(productions[i]);
        }
        cout << endl;
        return;
    }
    
    string newNonTerminal = nonTerminal + "'";
    
    // Output the modified rule for nonTerminal
    cout << nonTerminal << " -> ";
    for (size_t i = 0; i < beta.size(); i++) {
        if (i > 0)
            cout << " | ";
        cout << beta[i] << " " << newNonTerminal;
    }
    cout << endl;
    
    // Output the new rule for the new nonTerminal, adding epsilon at the end
    cout << newNonTerminal << " -> ";
    for (size_t i = 0; i < alpha.size(); i++) {
        if (i > 0)
            cout << " | ";
        cout << alpha[i] << " " << newNonTerminal;
    }
    cout << " | epsilon" << endl; // using "epsilon" instead of "ε"
}

int main() {
    int n;
    cout << "Enter the number of grammar rules: ";
    cin >> n;
    cin.ignore();  // consume newline
    
    for (int i = 0; i < n; i++) {
        string line;
        cout << "Enter the grammar rule (e.g., A -> Aa | b): ";
        getline(cin, line);
        
        // Find the arrow "->" in the input rule
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) {
            cout << "Invalid grammar rule format.\n";
            continue;
        }
        
        // Extract the nonTerminal and the right-hand side productions
        string nonTerminal = trim(line.substr(0, arrowPos));
        string rhs = trim(line.substr(arrowPos + 2));
        
        // Split the productions separated by '|'
        vector<string> productions;
        stringstream ss(rhs);
        string production;
        while (getline(ss, production, '|')) {
            productions.push_back(trim(production));
        }
        
        removeLeftRecursion(nonTerminal, productions);
    }
    
    return 0;
}