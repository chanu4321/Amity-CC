#include <bits/stdc++.h>
using namespace std;

int checkTokens(const string &str) {
    stringstream s(str);
    string word;
    int count = 0;
    while (s >> word) {
        count++;
    }
    return count;
}

int main() {
    ifstream file("input.txt");
    if (!file) {
        cerr << "Error opening file. Make sure 'input.txt' is present.\n";
        return 1;
    }
    string content;
    string line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    int totalTokens = checkTokens(content);
    cout << totalTokens << endl;

    file.close();
    return 0;
}
