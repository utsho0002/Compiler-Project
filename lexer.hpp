#ifndef SUBORNO_LEXER_HPP
#define SUBORNO_LEXER_HPP

#include <bits/stdc++.h>

using namespace std;

// ============================================================
// MEMBER 1: TOKEN SYSTEM + LEXER
// ============================================================

// TOKENS
const string int_      = "INT_TYPE";       // সংখ্যা
const string bool_     = "BOOL_TYPE";      // সত্যতা
const string id_       = "IDENTIFIER";
const string int_lit   = "INTEGER_LITERAL";
const string true_lit  = "TRUE";           // সত্য
const string false_lit = "FALSE";          // মিথ্যা
const string ptr       = "PRINT";          // দেখাও
const string if_       = "IF";             // যদি
const string else_     = "ELSE";           // নাহলে
const string while_    = "WHILE";          // যতক্ষণ
const string and_      = "AND";            // এবং
const string or_       = "OR";             // অথবা

const string assign = "ASSIGNMENT";        // :
const string pls    = "PLUS";              // +
const string mns    = "MINUS";             // -
const string mt     = "MULTIPLICATION";    // *
const string div_   = "DIVISION";          // /

const string eq  = "EQUAL";                // ==
const string neq = "NOT_EQUAL";            // !=
const string lt  = "LESS";                 // <
const string lte = "LESS_EQUAL";            // <=
const string gt  = "GREATER";              // >
const string gte = "GREATER_EQUAL";         // >=

const string lpr = "LEFT_PAREN";           // (
const string rpr = "RIGHT_PAREN";          // )
const string lbr = "LEFT_BRACE";           // {
const string rbr = "RIGHT_BRACE";          // }
const string semi = "SEMICOLON";           // ;
const string newline_ = "NEWLINE";
const string eof = "EOF";

// TOKEN CLASS
class Token {
public:
    string type_;
    string value;
    int line;

    Token(string t = "", string v = "", int l = 0) {
        type_ = t;
        value = v;
        line = l;
    }
};

inline void printToken(const Token& t) {
    cout << "< " << t.type_;
    if (!t.value.empty()) {
        cout << " , " << t.value;
    }
    cout << " > line " << t.line << endl;
}

// LEXER
class Lexer {
private:
    string code;
    int pos;
    int line;
    vector<Token> tokens;
    vector<string> errors;

public:
    Lexer(string c) {
        code = c;
        pos = 0;
        line = 1;
    }

    char get_char() {
        if (pos < (int)code.size()) {
            return code[pos];
        }
        return '\0';
    }

    char peek_char() {
        if (pos + 1 < (int)code.size()) {
            return code[pos + 1];
        }
        return '\0';
    }

    void advance() {
        if (pos < (int)code.size()) {
            if (code[pos] == '\n') {
                line++;
            }
            pos++;
        }
    }

    bool is_word_start(char c) {
        unsigned char u = (unsigned char)c;
        return isalpha(u) || c == '_' || u >= 128;
    }

    bool is_word_part(char c) {
        unsigned char u = (unsigned char)c;
        return isalnum(u) || c == '_' || u >= 128;
    }

    vector<Token> tokenize() {
        while (true) {
            char c = get_char();

            if (c == '\0') {
                break;
            }

            // Space/tab
            else if (c == ' ' || c == '\t' || c == '\r') {
                advance();
            }

            // Keep newline
            else if (c == '\n') {
                tokens.push_back(Token(newline_, "\\n", line));
                advance();
            }

            // Number
            else if (c >= '0' && c <= '9') {
                string num = "";

                while (get_char() >= '0' && get_char() <= '9') {
                    num += get_char();
                    advance();
                }

                tokens.push_back(Token(int_lit, num, line));
            }

            // Bangla/English keyword or identifier
            else if (is_word_start(c)) {
                string word = "";

                while (get_char() != '\0' && is_word_part(get_char())) {
                    word += get_char();
                    advance();
                }

                if (word == "সংখ্যা") {
                    tokens.push_back(Token(int_, word, line));
                }
                else if (word == "সত্যতা") {
                    tokens.push_back(Token(bool_, word, line));
                }
                else if (word == "দেখাও") {
                    tokens.push_back(Token(ptr, word, line));
                }
                else if (word == "যদি") {
                    tokens.push_back(Token(if_, word, line));
                }
                else if (word == "নাহলে") {
                    tokens.push_back(Token(else_, word, line));
                }
                else if (word == "যতক্ষণ") {
                    tokens.push_back(Token(while_, word, line));
                }
                else if (word == "এবং") {
                    tokens.push_back(Token(and_, word, line));
                }
                else if (word == "অথবা") {
                    tokens.push_back(Token(or_, word, line));
                }
                else if (word == "সত্য") {
                    tokens.push_back(Token(true_lit, word, line));
                }
                else if (word == "মিথ্যা") {
                    tokens.push_back(Token(false_lit, word, line));
                }
                else {
                    tokens.push_back(Token(id_, word, line));
                }
            }

            // Comment
            else if (c == '/' && peek_char() == '/') {
                while (get_char() != '\n' && get_char() != '\0') {
                    advance();
                }
            }

            // Operators and punctuation
            else if (c == ':') {
                tokens.push_back(Token(assign, ":", line));
                advance();
            }
            else if (c == '+') {
                tokens.push_back(Token(pls, "+", line));
                advance();
            }
            else if (c == '-') {
                tokens.push_back(Token(mns, "-", line));
                advance();
            }
            else if (c == '*') {
                tokens.push_back(Token(mt, "*", line));
                advance();
            }
            else if (c == '/') {
                tokens.push_back(Token(div_, "/", line));
                advance();
            }
            else if (c == '(') {
                tokens.push_back(Token(lpr, "(", line));
                advance();
            }
            else if (c == ')') {
                tokens.push_back(Token(rpr, ")", line));
                advance();
            }
            else if (c == '{') {
                tokens.push_back(Token(lbr, "{", line));
                advance();
            }
            else if (c == '}') {
                tokens.push_back(Token(rbr, "}", line));
                advance();
            }
            else if (c == ';') {
                tokens.push_back(Token(semi, ";", line));
                advance();
            }
            else if (c == '=' && peek_char() == '=') {
                tokens.push_back(Token(eq, "==", line));
                advance();
                advance();
            }
            else if (c == '!' && peek_char() == '=') {
                tokens.push_back(Token(neq, "!=", line));
                advance();
                advance();
            }
            else if (c == '<' && peek_char() == '=') {
                tokens.push_back(Token(lte, "<=", line));
                advance();
                advance();
            }
            else if (c == '>' && peek_char() == '=') {
                tokens.push_back(Token(gte, ">=", line));
                advance();
                advance();
            }
            else if (c == '<') {
                tokens.push_back(Token(lt, "<", line));
                advance();
            }
            else if (c == '>') {
                tokens.push_back(Token(gt, ">", line));
                advance();
            }
            else {
                errors.push_back(
                    "LexerError: Illegal Char '" + string(1, c) +
                    "' at line " + to_string(line)
                );
                advance();
            }
        }

        tokens.push_back(Token(eof, "", line));
        return tokens;
    }

    vector<string> getErrors() {
        return errors;
    }
};

#endif // SUBORNO_LEXER_HPP
