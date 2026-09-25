%code requires
{
class ASTNode;
}

%{

#include <iostream>
#include <string>
#include <cstring>
#include "AST.cpp"

using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *s);

ASTNode *root = nullptr;

%}

%union
{
    char *str;
    ASTNode *node;
}

%token TYPE_INT TYPE_DECIMAL TYPE_STRING TYPE_BOOL
%token TRUE_LIT FALSE_LIT

%token IF ELSE WHILE PRINT
%token BREAK CONTINUE

%token AND OR
%token ASSIGN

%token PLUS MINUS STAR SLASH

%token EQUALS NOT_EQUALS
%token LESS LESS_EQUAL GREATER GREATER_EQUAL

%token NOT

%token LPAREN RPAREN
%token LBRACE RBRACE
%token SEMICOLON

%token <str> IDENTIFIER
%token <str> INT_LITERAL
%token <str> DECIMAL_LITERAL
%token <str> STRING_LITERAL

%type <node> program statement_list statement
%type <node> declaration assignment print_stmt
%type <node> if_stmt while_stmt block
%type <node> expression logical_or logical_and equality relational
%type <node> additive multiplicative unary primary

%type <str> data_type

%left OR
%left AND
%left EQUALS NOT_EQUALS
%left LESS LESS_EQUAL GREATER_EQUAL GREATER
%left PLUS MINUS
%left STAR SLASH
%right NOT
%right UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%

program
:
statement_list
{
    root = new ASTNode("ProgramNode");

    for(auto &c : $1->children)
        root->children.push_back(c);

    delete $1;
    $$ = root;
}
;

statement_list
:
{
    $$ = new ASTNode("BlockNode");
}
|
statement_list statement
{
    if($2)
        $1->children.push_back(*$2);

    delete $2;
    $$ = $1;
}
;

statement
:
declaration { $$ = $1; }
| assignment { $$ = $1; }
| print_stmt { $$ = $1; }
| if_stmt { $$ = $1; }
| while_stmt { $$ = $1; }

| BREAK SEMICOLON
{
    $$ = new ASTNode(makeBreak());
}

| CONTINUE SEMICOLON
{
    $$ = new ASTNode(makeContinue());
}
;

data_type
:
TYPE_INT
{
    $$ = strdup("int");
}
|
TYPE_DECIMAL
{
    $$ = strdup("decimal");
}
|
TYPE_STRING
{
    $$ = strdup("string");
}
|
TYPE_BOOL
{
    $$ = strdup("bool");
}
;

declaration
:
data_type IDENTIFIER ASSIGN expression SEMICOLON
{
    ASTNode n = makeDeclaration($1,$2);
    n.addChild(*$4);

    $$ = new ASTNode(n);

    free($1);
    free($2);
    delete $4;
}

|
data_type IDENTIFIER SEMICOLON
{
    ASTNode n = makeDeclaration($1,$2);

    $$ = new ASTNode(n);

    free($1);
    free($2);
}
;

assignment
:
IDENTIFIER ASSIGN expression SEMICOLON
{
    ASTNode n = makeAssignment($1);
    n.addChild(*$3);

    $$ = new ASTNode(n);

    free($1);
    delete $3;
}
;

print_stmt
:
PRINT LPAREN expression RPAREN SEMICOLON
{
    ASTNode n = makePrint();
    n.addChild(*$3);

    $$ = new ASTNode(n);

    delete $3;
}
;

if_stmt
:
IF LPAREN expression RPAREN block %prec LOWER_THAN_ELSE
{
    ASTNode n = makeIf();

    n.addChild(*$3);
    n.addChild(*$5);

    $$ = new ASTNode(n);

    delete $3;
    delete $5;
}

|
IF LPAREN expression RPAREN block ELSE block
{
    ASTNode n = makeIf();

    n.addChild(*$3);
    n.addChild(*$5);
    n.addChild(*$7);

    $$ = new ASTNode(n);

    delete $3;
    delete $5;
    delete $7;
}
;

while_stmt
:
WHILE LPAREN expression RPAREN block
{
    ASTNode n = makeWhile();

    n.addChild(*$3);
    n.addChild(*$5);

    $$ = new ASTNode(n);

    delete $3;
    delete $5;
}
;

block
:
LBRACE statement_list RBRACE
{
    $$ = $2;
}
;

expression
:
logical_or { $$ = $1; }
;

logical_or
:
logical_and { $$ = $1; }

|
logical_or OR logical_and
{
    $$ = new ASTNode(makeBinary("অথবা",*$1,*$3));
    delete $1;
    delete $3;
}
;

logical_and
:
equality { $$ = $1; }

|
logical_and AND equality
{
    $$ = new ASTNode(makeBinary("এবং",*$1,*$3));
    delete $1;
    delete $3;
}
;

equality
:
relational { $$ = $1; }

|
equality EQUALS relational
{
    $$ = new ASTNode(makeBinary("==",*$1,*$3));
    delete $1;
    delete $3;
}

|
equality NOT_EQUALS relational
{
    $$ = new ASTNode(makeBinary("!=",*$1,*$3));
    delete $1;
    delete $3;
}
;

relational
:
additive { $$ = $1; }

|
relational LESS additive
{
    $$ = new ASTNode(makeBinary("<",*$1,*$3));
    delete $1;
    delete $3;
}

|
relational LESS_EQUAL additive
{
    $$ = new ASTNode(makeBinary("<=",*$1,*$3));
    delete $1;
    delete $3;
}

|
relational GREATER additive
{
    $$ = new ASTNode(makeBinary(">",*$1,*$3));
    delete $1;
    delete $3;
}

|
relational GREATER_EQUAL additive
{
    $$ = new ASTNode(makeBinary(">=",*$1,*$3));
    delete $1;
    delete $3;
}
;

additive
:
multiplicative { $$ = $1; }

|
additive PLUS multiplicative
{
    $$ = new ASTNode(makeBinary("+",*$1,*$3));
    delete $1;
    delete $3;
}

|
additive MINUS multiplicative
{
    $$ = new ASTNode(makeBinary("-",*$1,*$3));
    delete $1;
    delete $3;
}
;

multiplicative
:
unary { $$ = $1; }

|
multiplicative STAR unary
{
    $$ = new ASTNode(makeBinary("*",*$1,*$3));
    delete $1;
    delete $3;
}

|
multiplicative SLASH unary
{
    $$ = new ASTNode(makeBinary("/",*$1,*$3));
    delete $1;
    delete $3;
}
;

unary
:
NOT unary %prec NOT
{
    $$ = new ASTNode(makeUnary("!",*$2));
    delete $2;
}

|
MINUS unary %prec UMINUS
{
    $$ = new ASTNode(makeUnary("-",*$2));
    delete $2;
}

|
primary { $$ = $1; }
;

primary
:
INT_LITERAL
{
    $$ = new ASTNode(makeInt($1));
    free($1);
}

|
DECIMAL_LITERAL
{
    $$ = new ASTNode(makeDecimal($1));
    free($1);
}

|
STRING_LITERAL
{
    $$ = new ASTNode(makeString($1));
    free($1);
}

|
TRUE_LIT
{
    $$ = new ASTNode(makeBool("সত্য"));
}

|
FALSE_LIT
{
    $$ = new ASTNode(makeBool("মিথ্যা"));
}

|
IDENTIFIER
{
    $$ = new ASTNode(makeIdentifier($1));
    free($1);
}

|
LPAREN expression RPAREN
{
    $$ = $2;
}
;

%%

void yyerror(const char *s)
{
    cerr
    << "Syntax Error [Line "
    << yylineno
    << "]: "
    << s
    << endl;
}
