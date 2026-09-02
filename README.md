# Suborno

> **Suborno is a Bengali-syntax programming language compiler project developed in C++ for learning and demonstrating the fundamental phases of compiler construction.**

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B\&logoColor=white)
![Status](https://img.shields.io/badge/Status-In%20Development-orange)
![Project](https://img.shields.io/badge/Project-Compiler-success)

---

## Overview

**Suborno** is an educational compiler project that allows programs to be written using Bengali keywords while following familiar programming concepts such as variables, expressions, conditional statements, and loops.

The current implementation focuses primarily on the **front-end stages of compilation**, including lexical analysis, syntax analysis, AST construction, error recovery, and basic type checking.

---

## Current Features

* Lexical analysis and token generation
* Recursive-descent parser
* Abstract Syntax Tree (AST) generation
* Integer and Boolean data types
* Variable declaration and assignment
* Arithmetic operations: `+`, `-`, `*`, `/`
* Correct arithmetic operator precedence
* Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`
* Logical operators: `এবং`, `অথবা`
* `যদি–নাহলে` conditional statements
* `যতক্ষণ` loops
* Basic syntax-error recovery
* Basic semantic and type checking
* Bengali identifiers and keywords

---

## Example

```text
সংখ্যা ক : 5;
সংখ্যা খ : 2;

সত্যতা বড় : ক > খ;

যদি (বড়) {
    দেখাও ক + খ;
}
নাহলে {
    দেখাও খ;
}

যতক্ষণ (ক > 0) {
    ক : ক - 1;
}
```

---

## Compiler Architecture

```text
Source Program
      │
      ▼
┌─────────────┐
│    Lexer    │
└──────┬──────┘
       │ Tokens
       ▼
┌─────────────┐
│    Parser   │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│     AST     │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│ Type Checker│
└──────┬──────┘
       │
       ▼
 Code Generator
    (Planned)
```

---

## Language Keywords

| Suborno  | Equivalent |
| -------- | ---------- |
| `সংখ্যা` | Integer    |
| `সত্যতা` | Boolean    |
| `সত্য`   | True       |
| `মিথ্যা` | False      |
| `দেখাও`  | Print      |
| `যদি`    | If         |
| `নাহলে`  | Else       |
| `যতক্ষণ` | While      |
| `এবং`    | AND        |
| `অথবা`   | OR         |

---

## Build and Run

### Compile

```bash
g++ -std=c++17 main.cpp -o suborno
```

### Linux / macOS

```bash
./suborno
```

### Windows

```bash
suborno.exe
```

---

## Development Roadmap

* [x] Lexer
* [x] Token generation
* [x] Recursive-descent parser
* [x] AST generation
* [x] Operator precedence
* [x] Conditional statements
* [x] WHILE loop
* [x] Basic error recovery
* [x] Basic type checking
* [ ] Improved semantic analysis
* [ ] Strict statement validation
* [ ] Source-file support
* [ ] Target code generation
* [ ] Python / Java output
* [ ] Automated testing

---

## Project Objective

The objective of **Suborno** is to demonstrate how a programming language can be designed around Bengali syntax while implementing the essential stages of a compiler in a clear, structured, and extensible manner.

---

## Contributing

Contributions, suggestions, and improvements are welcome. Please create a new branch and submit a pull request with a clear description of the proposed changes.

---

## License

This project is intended primarily for **academic and educational purposes**.

---

<div align="center">

### সুবর্ণ — Programming Concepts in বাংলা

**Built with C++**

</div>
