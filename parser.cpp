#ifndef PARSER_CPP_INCLUDED
#define PARSER_CPP_INCLUDED

#include "lexer.cpp"


// AST NODE

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
// LL(1)-style Recursive Descent Parser

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

    // factor --> integer | bool | identifier | ( expression ) | - factor
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
