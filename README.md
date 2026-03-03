# Lisp Machine

Lisp-to-C compiler

### What is completed now

**Runtime (C)**
  - Types: int, float, string, symbol, pair, closure, nil
  - Memory: cons, first, rest
  - Arithmetic: +, -, *, /
  - Comparisons: =, <, >
  - Garbage collector: reference counting

**Parser (Python)**
  - Tokenizer: numbers, floats, strings, symbols, parens, quotes
  - AST nodes: Int, Float, String, Symbol, Nil, List, Quote, If
  - S-expression parser

**Code Generator (Python)**
  - C code generation for literals
  - Arithmetic and comparison operations
  - If expressions
  - List operations (cons, first, rest)
  - Print function

### Structure of project

```
src/
  parser/      - Lisp parser (tokenizer, AST)
  generator/   - C code generator
runtime/
  lisp.h       - Runtime header
  lisp.c       - Runtime implementation
tests/
  test_runtime.c
  test_parser.py
```

### Build

```bash
gcc -c runtime/lisp.c -o build/lisp.o
ar rcs build/liblisp.a build/lisp.o
```

### Usage

```bash
python3 src/main.py input.lisp -o output.c
gcc output.c -Lbuild -llisp -o program
```