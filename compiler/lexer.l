%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

vector<string> lexerTokens;


static string tokenName(int token)
{
    switch(token)
    {
        case TYPE_INT: return "TYPE_INT";
        case TYPE_DECIMAL: return "TYPE_DECIMAL";
        case TYPE_STRING: return "TYPE_STRING";
        case TYPE_BOOL: return "TYPE_BOOL";

        case TRUE_LIT: return "TRUE_LIT";
        case FALSE_LIT: return "FALSE_LIT";

        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case PRINT: return "PRINT";

        case BREAK: return "BREAK";
        case CONTINUE: return "CONTINUE";

        case AND: return "AND";
        case OR: return "OR";

        case ASSIGN: return "ASSIGN";

        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case STAR: return "STAR";
        case SLASH: return "SLASH";

        case EQUALS: return "EQUALS";
        case NOT_EQUALS: return "NOT_EQUALS";

        case LESS: return "LESS";
        case LESS_EQUAL: return "LESS_EQUAL";
        case GREATER: return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";

        case NOT: return "NOT";

        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";

        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";

        case SEMICOLON: return "SEMICOLON";

        case IDENTIFIER: return "IDENTIFIER";

        case INT_LITERAL: return "INT_LITERAL";
        case DECIMAL_LITERAL: return "DECIMAL_LITERAL";
        case STRING_LITERAL: return "STRING_LITERAL";

        default: return "UNKNOWN";
    }
}


static int emitToken(int token)
{
    lexerTokens.push_back(
        to_string(yylineno)
        + "\t"
        + tokenName(token)
        + "\t"
        + string(yytext)
    );

    return token;
}

%}

%option noyywrap
%option yylineno

%%

[ \t\r\n]+ ;

"//"[^\r\n]* ;
"##"[^\r\n]* ;


"সংখ্যা"       { return emitToken(TYPE_INT); }
"দশমিক"       { return emitToken(TYPE_DECIMAL); }
"স্ট্রিং"      { return emitToken(TYPE_STRING); }
"সত্যতা"      { return emitToken(TYPE_BOOL); }


"যদি"         { return emitToken(IF); }
"নাহলে"       { return emitToken(ELSE); }
"যতক্ষণ"      { return emitToken(WHILE); }
"দেখাও"       { return emitToken(PRINT); }

"বিরতি"       { return emitToken(BREAK); }
"চালিয়ে_যাও" { return emitToken(CONTINUE); }


"সত্য"        { return emitToken(TRUE_LIT); }
"মিথ্যা"      { return emitToken(FALSE_LIT); }


"এবং"         { return emitToken(AND); }
"অথবা"        { return emitToken(OR); }


"=="          { return emitToken(EQUALS); }
"!="          { return emitToken(NOT_EQUALS); }
"<="          { return emitToken(LESS_EQUAL); }
">="          { return emitToken(GREATER_EQUAL); }
"<"           { return emitToken(LESS); }
">"           { return emitToken(GREATER); }


"="           { return emitToken(ASSIGN); }


"+"           { return emitToken(PLUS); }
"-"           { return emitToken(MINUS); }
"*"           { return emitToken(STAR); }
"/"           { return emitToken(SLASH); }
"!"           { return emitToken(NOT); }


"("           { return emitToken(LPAREN); }
")"           { return emitToken(RPAREN); }

"{"           { return emitToken(LBRACE); }
"}"           { return emitToken(RBRACE); }

";"           { return emitToken(SEMICOLON); }


[0-9]+\.[0-9]+ {
    yylval.str = strdup(yytext);
    return emitToken(DECIMAL_LITERAL);
}


[0-9]+ {
    yylval.str = strdup(yytext);
    return emitToken(INT_LITERAL);
}


\"([^\"\\]|\\.)*\" {
    yylval.str = strdup(yytext);
    return emitToken(STRING_LITERAL);
}


([a-zA-Z_]|[\200-\377])([a-zA-Z0-9_]|[\200-\377])* {
    yylval.str = strdup(yytext);
    return emitToken(IDENTIFIER);
}


. {
    lexerTokens.push_back(
        to_string(yylineno)
        + "\tLEXER_ERROR\t"
        + string(yytext)
    );

    fprintf(stderr,
        "Lexer Error [Line %d]: Unknown symbol '%s'\n",
        yylineno,
        yytext
    );

    return yytext[0];
}

%%
