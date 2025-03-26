#include <bits/stdc++.h>
using namespace std;

stack <string> s1;
stack <string> s2;

int check(string str){
    stringstream s(str);
    string word;
    int count = 0;
    while (s>>word){
        count++;
        s1.push(word);
    }
    return count;

    while(!s1.empty()){
        s2.push(s1.top());
        s1.pop();
    }

    while(!s2.empty()){
        s2.pop();
    }
}

int main() {
    int count;
    string str = "bruh moment";
    count = check(str);
    cout << count << endl; 
    return 0;
}