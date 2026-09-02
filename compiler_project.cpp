#include <bits/stdc++.h>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;


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
const string lte = "LESS_EQUAL";           // <=
const string gt  = "GREATER";              // >
const string gte = "GREATER_EQUAL";        // >=

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

void printToken(const Token& t) {
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



//  AST NODE


class ASTNode {
public:
    string kind;
    string value;
    int line;
    vector<ASTNode> children;

    ASTNode(string k = "", string v = "", int l = 0) {
        kind = k;
        value = v;
        line = l;
    }
};

ASTNode IntNode(string value, int line) {
    return ASTNode("IntNode", value, line);
}

ASTNode BoolNode(string value, int line) {
    return ASTNode("BoolNode", value, line);
}

ASTNode IdNode(string value, int line) {
    return ASTNode("IdNode", value, line);
}

ASTNode BinOpNode(ASTNode left, string op, ASTNode right, int line) {
    ASTNode node("BinOpNode", op, line);
    node.children.push_back(left);
    node.children.push_back(right);
    return node;
}

ASTNode UnaryOpNode(string op, ASTNode operand, int line) {
    ASTNode node("UnaryOpNode", op, line);
    node.children.push_back(operand);
    return node;
}

ASTNode AssignNode(string id, ASTNode value, int line) {
    ASTNode node("AssignNode", id, line);
    node.children.push_back(value);
    return node;
}

ASTNode DecNode(string type, string id, int line) {
    return ASTNode("DecNode", type + " " + id, line);
}

ASTNode PrtNode(ASTNode expr, int line) {
    ASTNode node("PrtNode", "", line);
    node.children.push_back(expr);
    return node;
}

ASTNode ProgNode(vector<ASTNode> stmts) {
    ASTNode node("ProgNode");
    node.children = stmts;
    return node;
}

void tree(const ASTNode& node, int level = 0) {
    string pre = "";
    for (int i = 0; i < level; i++) {
        pre += "---";
    }

    cout << pre << node.kind;
    if (!node.value.empty()) {
        cout << "(" << node.value << ")";
    }
    cout << endl;

    for (const ASTNode& child : node.children) {
        tree(child, level + 1);
    }
}


// PARSER


class Parser {
private:
    vector<Token> tokens;
    int pos;
    vector<string> errors;

public:
    Parser(vector<Token> t) {
        tokens = t;
        pos = 0;
    }

    Token currentTok() {
        if (pos < (int)tokens.size()) {
            return tokens[pos];
        }
        return Token(eof, "", 0);
    }

    void advance() {
        if (pos < (int)tokens.size() - 1) {
            pos++;
        }
    }

    void skipSeparators() {
        while (currentTok().type_ == newline_ || currentTok().type_ == semi) {
            advance();
        }
    }

    Token expect(string type, string message) {
        Token tok = currentTok();

        if (tok.type_ == type) {
            advance();
            return tok;
        }

        throw runtime_error(
            "ParserError at line " + to_string(tok.line) +
            ": " + message
        );
    }

    void synchronize() {
      
        while (currentTok().type_ != eof) {
            if (currentTok().type_ == semi || currentTok().type_ == newline_) {
                advance();
                return;
            }

            if (currentTok().type_ == rbr) {
                return;
            }

            advance();
        }
    }

    // factor --> integer | bool | identifier | ( expression )
    ASTNode factor() {
        Token tok = currentTok();

        if (tok.type_ == int_lit) {
            advance();
            return IntNode(tok.value, tok.line);
        }
        else if (tok.type_ == true_lit || tok.type_ == false_lit) {
            advance();
            return BoolNode(tok.value, tok.line);
        }
        else if (tok.type_ == id_) {
            advance();
            return IdNode(tok.value, tok.line);
        }
        else if (tok.type_ == lpr) {
            advance();
            ASTNode node = expression();
            expect(rpr, "expected ')' ");
            return node;
        }
        else if (tok.type_ == mns) {
            advance();
            ASTNode operand = factor();
            return UnaryOpNode("-", operand, tok.line);
    }           

        throw runtime_error(
            "ParserError at line " + to_string(tok.line) +
            ": unexpected token " + tok.type_
        );
    }

    // term --> factor { (* | /) factor }
    ASTNode term() {
        ASTNode left = factor();

        while (currentTok().type_ == mt || currentTok().type_ == div_) {
            Token op = currentTok();
            advance();

            ASTNode right = factor();
            left = BinOpNode(left, op.value, right, op.line);
        }

        return left;
    }

    // expr --> term { (+ | -) term }
    ASTNode expr() {
        ASTNode left = term();

        while (currentTok().type_ == pls || currentTok().type_ == mns) {
            Token op = currentTok();
            advance();

            ASTNode right = term();
            left = BinOpNode(left, op.value, right, op.line);
        }

        return left;
    }

    // comparison --> expr [ comparison_operator expr ]
    ASTNode comparison() {
        ASTNode left = expr();

        string t = currentTok().type_;
        if (t == eq || t == neq || t == lt || t == lte || t == gt || t == gte) {
            Token op = currentTok();
            advance();

            ASTNode right = expr();
            left = BinOpNode(left, op.value, right, op.line);
        }

        return left;
    }

    // logical_not --> comparison
    ASTNode logical_not() {
        return comparison();
    }

    // logical_and --> logical_not { এবং logical_not }
    ASTNode logical_and() {
        ASTNode left = logical_not();

        while (currentTok().type_ == and_) {
            Token op = currentTok();
            advance();

            ASTNode right = logical_not();
            left = BinOpNode(left, op.value, right, op.line);
        }

        return left;
    }

    // expression --> logical_and { অথবা logical_and }
    ASTNode expression() {
        ASTNode left = logical_and();

        while (currentTok().type_ == or_) {
            Token op = currentTok();
            advance();

            ASTNode right = logical_and();
            left = BinOpNode(left, op.value, right, op.line);
        }

        return left;
    }

    // declaration --> (সংখ্যা | সত্যতা) identifier [ : expression ]
    ASTNode dec_stmt() {
        Token typeTok = currentTok();
        advance();

        Token idTok = expect(id_, "expected identifier after data type");

        string typeName;
        if (typeTok.type_ == int_) {
            typeName = "int";
        }
        else {
            typeName = "bool";
        }

        ASTNode node = DecNode(typeName, idTok.value, typeTok.line);

        if (currentTok().type_ == assign) {
            advance();
            node.children.push_back(expression());
        }

        return node;
    }

    // assignment --> identifier : expression
    ASTNode assign_stmt() {
        Token idTok = currentTok();
        advance();

        expect(assign, "expected ':' after identifier");
        ASTNode value = expression();

        return AssignNode(idTok.value, value, idTok.line);
    }

    // print --> দেখাও expression
    ASTNode prt_stmt() {
        Token tok = currentTok();
        advance();

        ASTNode value = expression();
        return PrtNode(value, tok.line);
    }

    // block --> { statements }
    ASTNode block() {
        Token brace = expect(lbr, "expected '{'");
        ASTNode node("BlockNode", "", brace.line);

        skipSeparators();

        while (currentTok().type_ != rbr && currentTok().type_ != eof) {
            try {
                node.children.push_back(stmt());
            }
            catch (runtime_error& e) {
                errors.push_back(e.what());
                synchronize();
            }

            skipSeparators();
        }

        expect(rbr, "expected '}'");
        return node;
    }

    // if --> যদি ( expression ) block [ নাহলে block ]
    ASTNode if_stmt() {
        Token tok = currentTok();
        advance();

        expect(lpr, "expected '(' after যদি");
        ASTNode condition = expression();
        expect(rpr, "expected ')' after condition");

        skipSeparators();
        ASTNode ifBlock = block();

        ASTNode node("IfNode", "", tok.line);
        node.children.push_back(condition);
        node.children.push_back(ifBlock);

        skipSeparators();

        if (currentTok().type_ == else_) {
            advance();
            skipSeparators();
            node.children.push_back(block());
        }

        return node;
    }

    // while --> যতক্ষণ ( expression ) block
    ASTNode while_stmt() {
        Token tok = currentTok();
        advance();

        expect(lpr, "expected '(' after যতক্ষণ");
        ASTNode condition = expression();
        expect(rpr, "expected ')' after condition");

        skipSeparators();
        ASTNode body = block();

        ASTNode node("WhileNode", "", tok.line);
        node.children.push_back(condition);
        node.children.push_back(body);

        return node;
    }

    // statement --> declaration | assignment | print | if | while
    ASTNode stmt() {
        Token tok = currentTok();

        if (tok.type_ == int_ || tok.type_ == bool_) {
            return dec_stmt();
        }
        else if (tok.type_ == id_) {
            return assign_stmt();
        }
        else if (tok.type_ == ptr) {
            return prt_stmt();
        }
        else if (tok.type_ == if_) {
            return if_stmt();
        }
        else if (tok.type_ == while_) {
            return while_stmt();
        }

        throw runtime_error(
            "ParserError at line " + to_string(tok.line) +
            ": unexpected token " + tok.type_
        );
    }

    // program --> { statement }
    ASTNode program() {
        vector<ASTNode> stmts;
        skipSeparators();

        while (currentTok().type_ != eof) {
            try {
                stmts.push_back(stmt());
            }
            catch (runtime_error& e) {
                errors.push_back(e.what());
                synchronize();

                if (currentTok().type_ == rbr) {
                    advance();
                }
            }

            skipSeparators();
        }

        return ProgNode(stmts);
    }

    ASTNode parse() {
        return program();
    }

    vector<string> getErrors() {
        return errors;
    }
};



// TYPE CHECKER


class TypeChecker {
private:
    map<string, string> table;
    vector<string> errors;

    void error(int line, string msg) {
        errors.push_back("TypeError at line " + to_string(line) + ": " + msg);
    }

    string getExpressionType(const ASTNode& node) {
        if (node.kind == "IntNode") {
            return "int";
        }

        if (node.kind == "BoolNode") {
            return "bool";
        }

        if (node.kind == "IdNode") {
            if (table.find(node.value) == table.end()) {
                error(node.line, "variable '" + node.value + "' is not declared");
                return "error";
            }
            return table[node.value];
        }

        if (node.kind == "BinOpNode") {
            string left = getExpressionType(node.children[0]);
            string right = getExpressionType(node.children[1]);
            string op = node.value;

            if (op == "+" || op == "-" || op == "*" || op == "/") {
                if (left != "error" && left != "int") {
                    error(node.line, "left side of '" + op + "' must be int");
                }
                if (right != "error" && right != "int") {
                    error(node.line, "right side of '" + op + "' must be int");
                }
                if (left == "error" || right == "error") return "error";
                return "int";
            }

            // <, >, <=, >= need integers and return bool.
            if (op == "<" || op == ">" || op == "<=" || op == ">=") {
                if (left != "error" && left != "int") {
                    error(node.line, "left side of comparison must be int");
                }
                if (right != "error" && right != "int") {
                    error(node.line, "right side of comparison must be int");
                }
                if (left == "error" || right == "error") return "error";
                return "bool";
            }

            // == and != need the same type and return bool.
            if (op == "==" || op == "!=") {
                if (left != "error" && right != "error" && left != right) {
                    error(node.line, "both sides of '" + op + "' must have same type");
                }
                if (left == "error" || right == "error") return "error";
                return "bool";
            }
        }

        return "error";
    }

    void checkNode(const ASTNode& node) {
        if (node.kind == "ProgNode" || node.kind == "BlockNode") {
            for (const ASTNode& child : node.children) {
                checkNode(child);
            }
            return;
        }

        if (node.kind == "DecNode") {
            int space = node.value.find(' ');
            string type = node.value.substr(0, space);
            string name = node.value.substr(space + 1);

            if (table.find(name) != table.end()) {
                error(node.line, "variable '" + name + "' already declared");
                return;
            }

            table[name] = type;

            if (!node.children.empty()) {
                string valueType = getExpressionType(node.children[0]);
                if (valueType != "error" && valueType != type) {
                    error(node.line, "cannot assign " + valueType + " to " + type + " variable '" + name + "'");
                }
            }
            return;
        }

        if (node.kind == "AssignNode") {
            string name = node.value;

            if (table.find(name) == table.end()) {
                error(node.line, "variable '" + name + "' is not declared");
                return;
            }

            string valueType = getExpressionType(node.children[0]);
            if (valueType != "error" && valueType != table[name]) {
                error(node.line, "cannot assign " + valueType + " to " + table[name] + " variable '" + name + "'");
            }
            return;
        }

        if (node.kind == "PrtNode") {
            getExpressionType(node.children[0]);
            return;
        }

        if (node.kind == "IfNode") {
            string conditionType = getExpressionType(node.children[0]);
            if (conditionType != "error" && conditionType != "bool") {
                error(node.line, "যদি condition must be bool");
            }

            for (int i = 1; i < (int)node.children.size(); i++) {
                checkNode(node.children[i]);
            }
            return;
        }

        if (node.kind == "WhileNode") {
            string conditionType = getExpressionType(node.children[0]);
            if (conditionType != "error" && conditionType != "bool") {
                error(node.line, "যতক্ষণ condition must be bool");
            }

            checkNode(node.children[1]);
            return;
        }
    }

public:
    void check(const ASTNode& root) {
        table.clear();
        errors.clear();
        checkNode(root);
    }

    vector<string> getErrors() {
        return errors;
    }
};



int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    string code = R"(
সংখ্যা ক : -5;
সংখ্যা খ : 2;

দেখাও ক + খ;
দেখাও ক - খ;
দেখাও ক * খ;
দেখাও ক / খ;

সত্যতা গ : ক == খ;
সত্যতা ঘ : ক != খ;
সত্যতা ঙ : ক < খ;
সত্যতা চ : ক <= খ;
সত্যতা ছ : ক > খ;
সত্যতা জ : ক >= খ;

সত্যতা ঝ : সত্য এবং মিথ্যা;
সত্যতা ঞ : সত্য অথবা মিথ্যা;

যদি (ক > খ) {
    দেখাও সত্য;
}
নাহলে {
    দেখাও মিথ্যা;
}

যতক্ষণ (ক > 0) {
    ক : ক - 1;
}
    
)";

    // Lexer
    Lexer l(code);
    vector<Token> tokens = l.tokenize();

    cout << "========== TOKENS ==========" << endl;
    for (Token t : tokens) {
        if (t.type_ != newline_) {
            printToken(t);
        }
    }

    for (string e : l.getErrors()) {
        cout << e << endl;
    }

    // Parser
    Parser p(tokens);
    ASTNode ast = p.parse();

    cout << "\n========== PARSE TREE ==========" << endl;
    tree(ast);

    for (string e : p.getErrors()) {
        cout << e << endl;
    }

    // Type checker
    TypeChecker checker;
    checker.check(ast);

    cout << "\n========== TYPE CHECK ==========" << endl;
    vector<string> typeErrors = checker.getErrors();

    if (typeErrors.empty()) {
        cout << "No type errors." << endl;
    }
    else {
        for (string e : typeErrors) {
            cout << e << endl;
        }
    }

    return 0;
}