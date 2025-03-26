#include <iostream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct Rule {
    string nonTerminal;
    vector<string> productions;
};

// Trim leading and trailing whitespace.
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

// Returns the longest common prefix between two strings.
string getLongestCommonPrefix(const string &s1, const string &s2) {
    size_t i = 0;
    while(i < s1.size() && i < s2.size() && s1[i] == s2[i])
        i++;
    return s1.substr(0, i);
}

// This function factors a rule if two or more productions share a common (nonempty) prefix.
// Any new rules are added to newRules, and the function returns true if factoring occurred.
bool leftFactorRule(Rule &rule, vector<Rule> &newRules, int &counter) {
    vector<bool> processed(rule.productions.size(), false);
    vector<string> newProds;
    bool factored = false;
    
    // Process each production.
    for (size_t i = 0; i < rule.productions.size(); i++) {
        if (processed[i])
            continue;
        vector<size_t> group;
        group.push_back(i);
        processed[i] = true;
        // Group with any other production sharing a nonempty prefix.
        for (size_t j = i + 1; j < rule.productions.size(); j++) {
            if (!processed[j]) {
                string common = getLongestCommonPrefix(rule.productions[i], rule.productions[j]);
                if(common.size() > 0) {
                    group.push_back(j);
                    processed[j] = true;
                }
            }
        }
        if (group.size() > 1) {
            factored = true;
            // Determine the longest common prefix for the group.
            string commonPrefix = rule.productions[group[0]];
            for (size_t idx : group)
                commonPrefix = getLongestCommonPrefix(commonPrefix, rule.productions[idx]);
            
            // Create a new nonterminal name (e.g., A'1).
            string newNT = rule.nonTerminal + "'" + to_string(counter++);
            newProds.push_back(commonPrefix + newNT);
            
            // For each production in the group, record the remainder (or epsilon if empty).
            vector<string> newRuleProds;
            for (size_t idx : group) {
                string remainder = rule.productions[idx].substr(commonPrefix.size());
                remainder = trim(remainder);
                if (remainder == "")
                    remainder = "ε"; // Use epsilon if nothing remains.
                newRuleProds.push_back(remainder);
            }
            Rule newRule;
            newRule.nonTerminal = newNT;
            newRule.productions = newRuleProds;
            newRules.push_back(newRule);
        } else {
            // If no grouping, keep the production as is.
            newProds.push_back(rule.productions[i]);
        }
    }
    
    rule.productions = newProds;
    return factored;
}

int main(){
    int n;
    cout << "Enter the number of grammar rules: ";
    cin >> n;
    cin.ignore(); // Consume newline
    
    vector<Rule> grammar;
    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        if (trim(line) == "") {
            i--;
            continue;
        }
        size_t pos = line.find("->");
        if (pos == string::npos) {
            cerr << "Invalid grammar rule format.\n";
            continue;
        }
        string nonTerminal = trim(line.substr(0, pos));
        string rhs = line.substr(pos + 2);
        vector<string> prods;
        stringstream ss(rhs);
        string prod;
        while(getline(ss, prod, '|')) {
            string p = trim(prod);
            if (!p.empty())
                prods.push_back(p);
        }
        Rule rule;
        rule.nonTerminal = nonTerminal;
        rule.productions = prods;
        grammar.push_back(rule);
    }
    
    // Iteratively apply left factoring only on rules that haven't been factored yet.
    int counter = 1;
    bool changed = true;
    while(changed) {
        changed = false;
        vector<Rule> newRules;
        // Only factor rules that are original (i.e. without a prime in the nonterminal).
        for (size_t i = 0; i < grammar.size(); i++) {
            if (grammar[i].nonTerminal.find("'") != string::npos)
                continue;
            if (leftFactorRule(grammar[i], newRules, counter))
                changed = true;
        }
        // Append any new rules generated this round.
        for (auto &r : newRules)
            grammar.push_back(r);
    }
    
    cout << "\nLeft Factored Grammar:\n";
    for (auto &rule : grammar) {
        cout << rule.nonTerminal << " -> ";
        for (size_t i = 0; i < rule.productions.size(); i++) {
            if(i > 0)
                cout << " | ";
            cout << rule.productions[i];
        }
        cout << "\n";
    }
    
    return 0;
}
