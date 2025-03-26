#include <bits/stdc++.h>
using namespace std;
                   
int PrecedenceCheck(char ch){
    if (ch == '+' || ch == '-')
        return 1;
    else if (ch == '*' || ch == '/')
        return 2;
    else if (ch == '^')
        return 3;
}

bool isOperator(char ch){
    return ch == '+' || ch == '-'  || ch == '*' || ch == '/';
}

int main() {
    stack<char> stk;
    string infix;
    char ch;
    cin >> infix;

    stringstream exp(infix);
    string word;
    string postfix = "";
    for (int i = 0; i < infix.size(); ++i) {
        char ch = infix[i];
        if (isOperator(ch)) {
            while (!stk.empty() && PrecedenceCheck(stk.top()) >= PrecedenceCheck(ch) && stk.top() != '(') {
                postfix += stk.top();
                stk.pop();
            }
            stk.push(ch);
        }
        else if (ch == '('){
            stk.push(ch);
        }
        else if (ch == ')'){
            while(!stk.empty() && stk.top() != '('){
                postfix += stk.top();
                stk.pop();
            }
            if (!stk.empty() && stk.top() == '(')
                stk.pop();
        }
        else if (isalnum(ch)) {
            postfix +=ch;
        }
    }


    while (!stk.empty()) {
        postfix += stk.top();
        stk.pop();
    }

    cout << postfix << endl;
    return 0;
}