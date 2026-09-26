# Suborno Compiler & Learning Lab

Suborno is a small Bengali programming-language compiler presented as both:

1. an **online compiler** for writing, compiling and running Suborno programs, and
2. an **educational compiler visualizer** that exposes the real output of each compiler stage.

The web interface shows the full pipeline:

`Source -> Tokens -> Parse Tree / AST -> Semantic Analysis -> TAC -> Generated Python -> Execution`

## Project structure

```text
suborno-compiler-education-railway/
|-- app.py
|-- Dockerfile
|-- railway.json
|-- requirements.txt
|-- build-local.sh
|-- build-local.ps1
|-- regenerate-parser.sh
|-- compiler/
|   |-- lexer.l
|   |-- parser.y
|   |-- AST.cpp
|   |-- semantic.cpp
|   |-- tac.cpp
|   |-- codegen.cpp
|   |-- compiler.cpp
|   `-- main.cpp
|-- examples/
|   `-- integration.bn
`-- templates/
    `-- index.html
```

## Why generated files are not included

The following files are **generated build artifacts**, so they are intentionally not stored in the source project:

```text
lex.yy.c
parser.tab.c
parser.tab.h
bin/bangla_compiler
```

During a Docker/Railway build, the project generates them automatically:

```text
lexer.l  --flex-->  lex.yy.c
parser.y --bison--> parser.tab.c + parser.tab.h
                         |
                         v
              g++ builds bangla_compiler
```

The compiler also creates these stage files temporarily for each web request:

```text
tokens.txt
parse_tree.txt
semantic.txt
tac.txt
output.py
```

They are read by `app.py` and returned to the browser, then the temporary working directory is deleted.

## Compiler stages shown on the website

### 1. Lexical analysis
Flex reads the source program and records the token stream with line number, token type and lexeme.

### 2. Syntax analysis
Bison validates the token stream against the grammar and the compiler prints the resulting AST in tree form.

### 3. Semantic analysis
The semantic analyzer checks declarations, initialization and type compatibility and prints its symbol table.

### 4. Intermediate code generation
The validated AST is transformed into Three-Address Code (TAC).

### 5. Code generation
The TAC generator output is translated into Python target code.

### 6. Execution
The generated Python is executed with a timeout and its program output is shown in the online compiler section.

## Deploy directly to Railway without GitHub

Install the Railway CLI on your computer if it is not already installed:

```bash
npm install -g @railway/cli
```

Extract this project, open a terminal inside the project folder, then run:

```bash
railway login
railway init
railway up
```

Railway will use the included `Dockerfile`. The Docker build installs Flex and Bison, generates the lexer/parser files, compiles the C++ compiler, installs the Python dependencies and starts Gunicorn.

No GitHub repository is required for this workflow.

## Build locally on Linux/macOS

Requirements:

- `g++`
- `flex`
- `bison`
- Python 3

Run:

```bash
./build-local.sh
pip install -r requirements.txt
python app.py
```

The local build script generates parser/lexer files under `build/` and creates the executable under `bin/`. Both directories are ignored source artifacts.

## Build locally on Windows

Install Flex, Bison and a C++ compiler and ensure `flex`, `bison` and `g++` are available in `PATH`, then run PowerShell:

```powershell
./build-local.ps1
pip install -r requirements.txt
python app.py
```

## Deployment health check

The application exposes:

```text
GET /health
```

A healthy deployment returns JSON indicating that the web app is running and the native compiler binary exists.
