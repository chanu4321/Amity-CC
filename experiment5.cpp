#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
using namespace std;

// ---------- Helper Functions ----------

// Splits string s by delimiter delim.
vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    istringstream iss(s);
    string token;
    while(getline(iss, token, delim)) {
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if(start != string::npos)
            tokens.push_back(token.substr(start, end - start + 1));
    }
    return tokens;
}
 
// ---------- Grammar Data Structures ----------
// Grammar: mapping nonterminal -> list of alternatives (each alternative is a vector of symbols)
using Grammar = map<string, vector<vector<string>>>;
Grammar grammar;
// Set of nonterminals (those appearing on the LHS)
set<string> nonterminals;
 
// Reads the grammar from input.
void readGrammar() {
    int n;
    cout << "Enter the number of productions: ";
    cin >> n;
    cin.ignore(); // consume newline
    cout << "Enter productions (one per line) in the format: A -> α1 | α2\n";
    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        size_t pos = line.find("->");
        if(pos == string::npos) continue;
        string lhs = line.substr(0, pos);
        // Trim lhs:
        size_t start = lhs.find_first_not_of(" \t");
        size_t end = lhs.find_last_not_of(" \t");
        lhs = lhs.substr(start, end - start + 1);
        nonterminals.insert(lhs);
 
        string rhs = line.substr(pos + 2);
        vector<string> alternatives = split(rhs, '|');
        for(auto &alt : alternatives) {
            vector<string> symbols = split(alt, ' ');
            grammar[lhs].push_back(symbols);
        }
    }
}
 
// ---------- FIRST and FOLLOW Computation ----------
 
map<string, set<string>> FIRST, FOLLOW;
 
// Returns true if sym is a nonterminal.
bool isNonterminal(const string &sym) {
    return nonterminals.count(sym) > 0;
}
 
// Compute FIRST sets.
void computeFIRST() {
    // Initialize FIRST sets.
    for(auto nt : nonterminals)
        FIRST[nt] = set<string>();
 
    bool changed = true;
    while(changed) {
        changed = false;
        // For each production A -> α.
        for(auto &p : grammar) {
            string A = p.first;
            for(auto &prod : p.second) {
                bool allEps = true;
                for(auto &sym : prod) {
                    if(sym == "ep") {
                        if(FIRST[A].insert("ep").second)
                            changed = true;
                        allEps = false;
                        break;
                    }
                    if(!isNonterminal(sym)) { // terminal
                        if(FIRST[A].insert(sym).second)
                            changed = true;
                        allEps = false;
                        break;
                    } else {
                        // Add FIRST(sym) except ep.
                        for(auto t : FIRST[sym]) {
                            if(t != "ep" && FIRST[A].insert(t).second)
                                changed = true;
                        }
                        if(FIRST[sym].find("ep") == FIRST[sym].end()){
                            allEps = false;
                            break;
                        }
                    }
                }
                if(allEps) {
                    if(FIRST[A].insert("ep").second)
                        changed = true;
                }
            }
        }
    }
}
 
// Compute FOLLOW sets.
void computeFOLLOW(const string &startSymbol) {
    // Initialize FOLLOW sets.
    for(auto nt : nonterminals)
        FOLLOW[nt] = set<string>();
    FOLLOW[startSymbol].insert("$"); // add end-marker
 
    bool changed = true;
    while(changed) {
        changed = false;
        // For each production A -> α.
        for(auto &p : grammar) {
            string A = p.first;
            for(auto &prod : p.second) {
                for(size_t i = 0; i < prod.size(); i++) {
                    string B = prod[i];
                    if(isNonterminal(B)) {
                        bool allEps = true;
                        // Look at β (symbols after B).
                        for(size_t j = i + 1; j < prod.size(); j++){
                            string beta = prod[j];
                            if(!isNonterminal(beta)) {
                                if(FOLLOW[B].insert(beta).second)
                                    changed = true;
                                allEps = false;
                                break;
                            } else {
                                for(auto t : FIRST[beta]) {
                                    if(t != "ep" && FOLLOW[B].insert(t).second)
                                        changed = true;
                                }
                                if(FIRST[beta].find("ep") == FIRST[beta].end()){
                                    allEps = false;
                                    break;
                                }
                            }
                        }
                        if(allEps) {
                            // If β is empty or all derive ep, add FOLLOW(A) to FOLLOW(B).
                            for(auto t : FOLLOW[A]) {
                                if(FOLLOW[B].insert(t).second)
                                    changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}
 
// ---------- Building the Predictive Parsing Table ----------
// Table: table[nonterminal][terminal] = production (vector<string>)
using ParseTable = map<string, map<string, vector<string>>>;
ParseTable table;
 
// Compute FIRST of an alternative α.
set<string> FIRSTofAlternative(const vector<string> &alpha) {
    set<string> result;
    bool allEps = true;
    for(auto &sym : alpha) {
        if(sym == "ep") {
            result.insert("ep");
            allEps = false;
            break;
        }
        if(!isNonterminal(sym)) {
            result.insert(sym);
            allEps = false;
            break;
        } else {
            for(auto t : FIRST[sym])
                if(t != "ep")
                    result.insert(t);
            if(FIRST[sym].find("ep") == FIRST[sym].end()){
                allEps = false;
                break;
            }
        }
    }
    if(allEps)
        result.insert("ep");
    return result;
}
 
// Build the LL(1) parsing table.
void buildParseTable() {
    for(auto &p : grammar) {
        string A = p.first;
        for(auto &prod : p.second) {
            set<string> firstAlpha = FIRSTofAlternative(prod);
            // For every terminal in FIRST(α) except ep.
            for(auto t : firstAlpha) {
                if(t != "ep")
                    table[A][t] = prod;
            }
            // If ep is in FIRST(α), then for every terminal in FOLLOW(A), add A -> α.
            if(firstAlpha.find("ep") != firstAlpha.end()){
                for(auto t : FOLLOW[A])
                    table[A][t] = prod;
            }
        }
    }
}
 
// ---------- Utility for Debug Printing ----------
void printSet(const set<string> &s) {
    cout << "{ ";
    for(auto x : s)
        cout << x << " ";
    cout << "}";
}
 
// ---------- Main Function ----------
int main(){
    // 1. Read Grammar.
    readGrammar();
    // Assume start symbol is the LHS of the first production.
    string startSymbol = grammar.begin()->first;
 
    // 2. Compute FIRST and FOLLOW sets.
    computeFIRST();
    computeFOLLOW(startSymbol);
 
    cout << "\nFIRST sets:\n";
    for(auto nt : nonterminals) {
        cout << nt << " : ";
        printSet(FIRST[nt]);
        cout << "\n";
    }
    cout << "\nFOLLOW sets:\n";
    for(auto nt : nonterminals) {
        cout << nt << " : ";
        printSet(FOLLOW[nt]);
        cout << "\n";
    }
 
    // 3. Build the Predictive Parsing Table.
    buildParseTable();
    cout << "\nPredictive Parsing Table:\n";
    for(auto nt : nonterminals) {
        cout << nt << ":\n";
        for(auto &entry : table[nt]) {
            cout << "  " << entry.first << " -> ";
            for(auto sym : entry.second)
                cout << sym << " ";
            cout << "\n";
        }
    }
 
    // 4. Parse an Input String.
    cout << "\nEnter input string to parse (tokens separated by space, e.g., id + id * id):\n";
    string input;
    getline(cin, input);
    vector<string> tokens = split(input, ' ');
    if(tokens.empty() || tokens.back() != "$")
        tokens.push_back("$");
 
    cout << "\nInput tokens:";
    for(auto t : tokens)
        cout << " [" << t << "]";
    cout << "\n\nParsing Process:\n";
 
    stack<string> st;
    st.push("$");
    st.push(startSymbol);
    int index = 0;
 
    while(!st.empty()){
        string top = st.top();
        string curr = tokens[index];
        cout << "Stack top: " << top << "    Current token: " << curr << "\n";
        if(top == "$" && curr == "$"){
            cout << "\nParsing Successful!\n";
            break;
        }
        if(!isNonterminal(top)) {
            if(top == curr) {
                st.pop();
                index++;
            } else {
                cout << "Error: expected " << top << ", got " << curr << "\n";
                break;
            }
        } else {
            if(table[top].count(curr)) {
                vector<string> prod = table[top][curr];
                cout << top << " -> ";
                for(auto s : prod)
                    cout << s << " ";
                cout << "\n";
                st.pop();
                if(!(prod.size() == 1 && prod[0] == "ep")){
                    for(int i = prod.size()-1; i >= 0; i--){
                        st.push(prod[i]);
                    }
                }
            } else {
                cout << "Error: no entry for (" << top << ", " << curr << ")\n";
                break;
            }
        }
    }
    if(!st.empty() || index != tokens.size())
        cout << "\nParsing Failed.\n";
 
    return 0;
}