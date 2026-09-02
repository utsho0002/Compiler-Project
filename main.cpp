#include "parser.hpp"

#ifdef _WIN32
#include <windows.h>
#endif



// or stop before a closing brace so block parsing can recover.
void Parser::synchronize() {
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

// declaration --> (সংখ্যা | সত্যতা) identifier [ : expression ]
ASTNode Parser::dec_stmt() {
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
ASTNode Parser::assign_stmt() {
    Token idTok = currentTok();
    advance();

    expect(assign, "expected ':' after identifier");
    ASTNode value = expression();

    return AssignNode(idTok.value, value, idTok.line);
}

// print --> দেখাও expression
ASTNode Parser::prt_stmt() {
    Token tok = currentTok();
    advance();

    ASTNode value = expression();
    return PrtNode(value, tok.line);
}

// block --> { statements }
ASTNode Parser::block() {
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
ASTNode Parser::if_stmt() {
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
ASTNode Parser::while_stmt() {
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
ASTNode Parser::stmt() {
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
ASTNode Parser::program() {
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

ASTNode Parser::parse() {
    return program();
}

vector<string> Parser::getErrors() {
    return errors;
}

// ============================================================
// TYPE CHECKER
// Preserved from the original project.
// ============================================================

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
