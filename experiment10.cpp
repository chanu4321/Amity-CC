#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <string>
#include <cctype>
#include <map>
using namespace std;

// Check if a character is an operator.
bool isOperator(const string &s) {
    return s == "+" || s == "-" || s == "*" || s == "/" || s == "^";
}

// Return precedence of operator; higher number means higher precedence.
int precedence(const string &op) {
    if(op == "^")
        return 3;
    else if(op == "*" || op == "/")
        return 2;
    else if(op == "+" || op == "-")
        return 1;
    return 0;
}

// Convert an infix expression (as tokens) to postfix using the shunting-yard algorithm.
vector<string> infixToPostfix(const vector<string>& tokens) {
    vector<string> output;
    stack<string> opStack;
    for (const auto &token : tokens) {
        if(isOperator(token)) {
            // Pop operators with higher or equal precedence (for left-associative ops)
            while (!opStack.empty() && isOperator(opStack.top()) &&
                   precedence(opStack.top()) >= precedence(token)) {
                output.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(token);
        }
        else if(token == "(") {
            opStack.push(token);
        }
        else if(token == ")") {
            while(!opStack.empty() && opStack.top() != "(") {
                output.push_back(opStack.top());
                opStack.pop();
            }
            if(!opStack.empty())
                opStack.pop(); // pop "("
        }
        else {
            // Operand
            output.push_back(token);
        }
    }
    // Pop any remaining operators.
    while(!opStack.empty()){
        output.push_back(opStack.top());
        opStack.pop();
    }
    return output;
}

// Tokenize the input string. Here we assume tokens are separated by spaces.
vector<string> tokenize(const string &input) {
    vector<string> tokens;
    istringstream iss(input);
    string token;
    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}

// Generate three-address code from a postfix expression.
// For each operator, pop two operands from the stack, generate a temporary variable,
// print the TAC instruction, and push the temporary back onto the stack.
void generateThreeAddressCode(const vector<string> &postfix) {
    stack<string> st;
    int tempCount = 1;
    cout << "\nThree-Address Code:\n";
    for (const auto &token : postfix) {
        if(isOperator(token)) {
            // Pop two operands (note: second popped is left operand)
            if(st.size() < 2) {
                cerr << "Error: insufficient operands for operator " << token << endl;
                return;
            }
            string operand2 = st.top();
            st.pop();
            string operand1 = st.top();
            st.pop();
            // Generate a new temporary variable.
            string temp = "t" + to_string(tempCount++);
            cout << temp << " = " << operand1 << " " << token << " " << operand2 << "\n";
            // Push the temporary variable onto the stack.
            st.push(temp);
        }
        else {
            // Operand: push onto the stack.
            st.push(token);
        }
    }
    // The final result is in st.top() if needed.
    if (!st.empty()) {
        cout << "\nResult stored in: " << st.top() << "\n";
    }
}

int main() {
    cout << "Enter an arithmetic expression (tokens separated by spaces):\n";
    string input;
    getline(cin, input);

    // Tokenize the input.
    vector<string> tokens = tokenize(input);
    
    // Convert infix tokens to postfix.
    vector<string> postfix = infixToPostfix(tokens);

    // Display postfix expression.
    cout << "\nPostfix Expression: ";
    for (const auto &token : postfix)
        cout << token << " ";
    cout << "\n";

    // Generate three-address code.
    generateThreeAddressCode(postfix);

    return 0;
}