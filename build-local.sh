#!/usr/bin/env sh
set -eu

command -v bison >/dev/null 2>&1 || { echo "bison is required" >&2; exit 1; }
command -v flex >/dev/null 2>&1 || { echo "flex is required" >&2; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "g++ is required" >&2; exit 1; }

mkdir -p build bin

bison -d -o build/parser.tab.c compiler/parser.y
flex -o build/lex.yy.c compiler/lexer.l

# lexer.l includes parser.tab.h by name, so put the generated header next to lex.yy.c.
g++ -std=c++17 -O2 -pipe \
    -Ibuild -Icompiler \
    build/lex.yy.c build/parser.tab.c compiler/main.cpp \
    -o bin/bangla_compiler

echo "Built bin/bangla_compiler"
echo "Generated build/lex.yy.c, build/parser.tab.c and build/parser.tab.h"
