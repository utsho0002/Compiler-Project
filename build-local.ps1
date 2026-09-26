$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Force -Path build | Out-Null
New-Item -ItemType Directory -Force -Path bin | Out-Null

bison -d -o build/parser.tab.c compiler/parser.y
flex -o build/lex.yy.c compiler/lexer.l

g++ -std=c++17 -O2 -Ibuild -Icompiler build/lex.yy.c build/parser.tab.c compiler/main.cpp -o bin/bangla_compiler.exe

Write-Host "Built bin/bangla_compiler.exe"
Write-Host "Generated build/lex.yy.c, build/parser.tab.c and build/parser.tab.h"
