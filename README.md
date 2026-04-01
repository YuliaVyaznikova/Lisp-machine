# Lisp-machine

Lisp-to-C transpiler with minimal C runtime, garbage collection, and tail call optimization.

## Features

- Minimal C runtime - only types, GC, and primitives on C; everything else on Lisp
- Hybrid garbage collector - reference counting + mark&sweep for cycles
- Tail call optimization - recursive functions compile to loops
- Macros - define syntax extensions with `defmacro`
- Lazy evaluation - infinite sequences via `lazy-cons`
- Closures - full lexical scoping support
- Variadic functions - `&rest` parameter support

## Usage

### Prerequisites
- Docker (with `gcc:latest` image)

### Compile and run a Lisp file
```powershell
docker run --rm -v ${PWD}:/app -w /app gcc:latest bash -c "
  mkdir -p build output/myprogram &&
  gcc -c runtime/lisp.c -I. -o build/lisp.o &&
  python3 src/main.py myprogram.lisp -o output/myprogram/myprogram.c &&
  gcc output/myprogram/myprogram.c build/lisp.o -I. -o output/myprogram/myprogram &&
  ./output/myprogram/myprogram
"
```

## Tests

### Run lisp tests (Lisp examples)
```powershell
docker run --rm -v ${PWD}:/app -w /app gcc:latest bash run_all_tests.sh
```

### Run all tests (C runtime, GC, Python parser, E2E)
```powershell
docker build -t lisp-machine .
docker run --rm lisp-machine
```

### Run single test
```powershell
docker run --rm -v ${PWD}:/app -w /app gcc:latest bash -c "
  mkdir -p build output/hello &&
  gcc -c runtime/lisp.c -I. -o build/lisp.o &&
  python3 src/main.py examples/hello.lisp -o output/hello/hello.c &&
  gcc output/hello/hello.c build/lisp.o -I. -o output/hello/hello &&
  ./output/hello/hello
"
```

## Language features

### Special forms
`define`, `lambda`, `if`, `quote`, `defmacro`, `quasiquote`, `unquote`, `apply`, `while`, `set!`

### Built-in primitives
`cons`, `first`, `rest`, `+`, `-`, `*`, `/`, `=`, `<`, `>`, `mod`, `abs`, `min`, `max`, `not`, `print`, `princ`, `open`, `close`, `gc-collect`, `gc-stats`

### Standard macros
`when`, `unless`, `cond`, `and`, `or`, `let`, `swap`, `inc`, `dec`, `assert`, `not-nil?`, `lazy-cons`

### Library functions
`map`, `filter`, `reduce`, `length`, `reverse`, `compose`, `curry`, `flip`, `zero?`, `even?`, `odd?`, `nil?`

## Architecture

```
Lisp source -> Parser -> Macro Expander -> Semantic Analyzer -> C Generator -> GCC
```

1. Parser - tokenizes and builds AST
2. Macro expander - recursively expands macros to fixed point
3. Semantic analyzer - checks all symbols are defined
4. Code generator - produces C code with TCO where possible

## Garbage collection

- Reference counting - immediate deallocation when refcount hits 0
- Mark&sweep - handles reference cycles (runs periodically in separate thread)
