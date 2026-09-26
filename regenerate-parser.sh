#!/usr/bin/env sh
set -eu

mkdir -p build
bison -d -o build/parser.tab.c compiler/parser.y
flex -o build/lex.yy.c compiler/lexer.l

echo "Generated files are in build/:"
echo "  build/parser.tab.c"
echo "  build/parser.tab.h"
echo "  build/lex.yy.c"
