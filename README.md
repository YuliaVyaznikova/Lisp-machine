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
  main.py           - entry point
  parser/
    tokenizer.py    - lexer
    ast_nodes.py    - AST node definitions
    parser.py       - S-expression parser
  generator/
    generator.py    - C code generator
runtime/
  lisp.h            - Runtime header (types, functions)
  lisp.c            - Runtime implementation
examples/
  hello.lisp        - print string
  arithmetic.lisp   - +, *, numbers
  comparisons.lisp  - =, <, >
  conditionals.lisp - if expressions
  lists.lisp        - cons, first, rest
  quotes.lisp       - quote (')
tests/
  test_runtime.c    - C runtime tests
  test_parser.py    - Python parser/generator tests
```

### Examples

| File | Description |
|------|-------------|
| `hello.lisp` | Print "Hello, World!" |
| `arithmetic.lisp` | Arithmetic operations (+, *) |
| `comparisons.lisp` | Comparison operators (=, <, >) |
| `conditionals.lisp` | If expressions |
| `lists.lisp` | List operations (cons, first, rest) |
| `quotes.lisp` | Quoting with ' |

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