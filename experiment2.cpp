#include <bits/stdc++.h>
using namespace std;

bool isOperatorDelimiterChar(char c) {
    static const char delims[] = "(){}[].,;+-*/%";
    return (strchr(delims, c) != nullptr);
}

bool isKeyword(const string& token) {
    static const unordered_set<string> keywords = {
        // Control flow
        "if", "else", "while", "for", "do", "switch", "case",
        "break", "continue", "return", "goto",
        // Types
        "int", "float", "double", "char", "bool", "void", "long", "short",
        "struct", "union", "enum", "class",
        // Others
        "public", "private", "protected", "static",
        "include", "using", "namespace", "std", "default", "delete",
        "signed", "unsigned", "const", "volatile", "extern", "register",
        "sizeof", "typeof", "typename", "template"
    };
    return (keywords.find(token) != keywords.end());
}

bool isIdentifier(const string& token) {
    if (token.empty()) return false;
    if (!isalpha(token[0]) && token[0] != '_') return false;
    for (size_t i = 1; i < token.size(); i++) {
        if (!isalnum(token[i]) && token[i] != '_')
            return false;
    }
    return true;
}

bool isIntegerLiteral(const string& token) {
    if (token.empty()) return false;
    for (char c : token) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

bool isCharLiteral(const string& token) {
    // Minimal check: exactly 3 characters: 'x'
    if (token.size() == 3 && token.front() == '\'' && token.back() == '\'')
        return true;
    return false;
}

bool isStringLiteral(const string& token) {
    // Minimal check: must have at least 2 characters, starting and ending with "
    if (token.size() >= 2 && token.front() == '\"' && token.back() == '\"')
        return true;
    return false;
}

bool isComment(const string& token) {
    if (token.empty()) return false;
    // Check for single-line comment
    if (token.size() >= 2 && token[0] == '/' && token[1] == '/')
        return true;
    // Check for start or end of multi-line comment
    if (token.size() >= 2 && token[0] == '/' && token[1] == '*')
        return true;
    if (token.size() >= 2 && token[0] == '*' && token[1] == '/')
        return true;
    return false;
}

bool isPreprocessorDirective(const string& token) {
    // Basic rule: token starts with '#'
    return (!token.empty() && token[0] == '#');
}

string categorizeToken(const string& token) {
    if (isComment(token))
        return "Comment";
    else if (isKeyword(token))
        return "Keyword";
    else if (isPreprocessorDirective(token))
        return "Preprocessor";
    else if (isIntegerLiteral(token))
        return "Literal";
    else if (isCharLiteral(token))
        return "CharLiteral";
    else if (isStringLiteral(token))
        return "StringLiteral";
    else if (isIdentifier(token))
        return "Identifier";
    return "Unknown";
}

int main() {
    // Vectors to store categorized tokens
    vector<string> keywords;
    vector<string> preprocessor;
    vector<string> identifiers;
    vector<string> literals;
    vector<string> charLiterals;
    vector<string> stringLiterals;
    vector<string> operatorsDelimiters;
    vector<string> unknowns;

    // Flag to indicate if we're inside a multi-line comment
    bool inMultiLineComment = false;

    ifstream file("input.txt");
    if (!file) {
        cerr << "Error: could not open input.txt\n";
        return 1;
    }

    // Helper lambda to flush and categorize a token
    auto flushToken = [&](string &token) {
        if (!token.empty()) {
            string cat = categorizeToken(token);
            if (cat == "Keyword")
                keywords.push_back(token);
            else if (cat == "Preprocessor")
                preprocessor.push_back(token);
            else if (cat == "Identifier")
                identifiers.push_back(token);
            else if (cat == "Literal")
                literals.push_back(token);
            else if (cat == "CharLiteral")
                charLiterals.push_back(token);
            else if (cat == "StringLiteral")
                stringLiterals.push_back(token);
            else if (cat == "Comment")
                ; // Ignore comments
            else
                unknowns.push_back(token);
            token.clear();
        }
    };

    // Helper lambda to process a segment of text (non-comment part)
    auto processSegment = [&](const string &segment) {
        string token;
        for (size_t i = 0; i < segment.size(); i++) {
            char c = segment[i];
            if (isspace(static_cast<unsigned char>(c)) || isOperatorDelimiterChar(c)) {
                flushToken(token);
                if (isOperatorDelimiterChar(c)) {
                    string op(1, c);
                    operatorsDelimiters.push_back(op);
                }
            } else {
                token.push_back(c);
            }
        }
        flushToken(token);
    };

    string line;
    while (getline(file, line)) {
        size_t pos = 0;
        while (pos < line.size()) {
            if (inMultiLineComment) {
                // Look for the end of a multi-line comment
                size_t endPos = line.find("*/", pos);
                if (endPos != string::npos) {
                    inMultiLineComment = false;
                    pos = endPos + 2;
                } else {
                    // Entire remainder of the line is within a comment
                    pos = line.size();
                }
            } else {
                // Look for comment markers
                size_t singleCommentPos = line.find("//", pos);
                size_t multiCommentPos = line.find("/*", pos);
                if (singleCommentPos == string::npos && multiCommentPos == string::npos) {
                    // No comments ahead; process the rest of the line
                    string segment = line.substr(pos);
                    processSegment(segment);
                    break; // done with this line
                } else {
                    if (multiCommentPos != string::npos &&
                        (singleCommentPos == string::npos || multiCommentPos < singleCommentPos)) {
                        // Process text before the multi-line comment
                        string segment = line.substr(pos, multiCommentPos - pos);
                        processSegment(segment);
                        inMultiLineComment = true;
                        pos = multiCommentPos + 2;
                    } else if (singleCommentPos != string::npos &&
                               (multiCommentPos == string::npos || singleCommentPos < multiCommentPos)) {
                        // Process text before the single-line comment
                        string segment = line.substr(pos, singleCommentPos - pos);
                        processSegment(segment);
                        // Skip rest of the line (it's a comment)
                        pos = line.size();
                    }
                }
            }
        }
    }
    file.close();

    // Print results by category
    cout << "\n--- Keywords ---\n";
    for (const auto &kw : keywords)
        cout << kw << '\n';

    cout << "\n--- Preprocessor Directives ---\n";
    for (const auto &pd : preprocessor)
        cout << pd << '\n';

    cout << "\n--- Identifiers ---\n";
    for (const auto &id : identifiers)
        cout << id << '\n';

    cout << "\n--- Integer Literals ---\n";
    for (const auto &lit : literals)
        cout << lit << '\n';

    cout << "\n--- Character Literals ---\n";
    for (const auto &ch : charLiterals)
        cout << ch << '\n';

    cout << "\n--- String Literals ---\n";
    for (const auto &str : stringLiterals)
        cout << str << '\n';

    cout << "\n--- Operators/Delimiters ---\n";
    for (const auto &op : operatorsDelimiters)
        cout << op << '\n';

    cout << "\n--- Unknown ---\n";
    for (const auto &unk : unknowns)
        cout << unk << '\n';

    return 0;
}
