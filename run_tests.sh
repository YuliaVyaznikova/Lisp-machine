#!/bin/bash
mkdir -p build output/bin
gcc -c runtime/lisp.c -I. -o build/lisp.o

for f in examples/*.lisp; do
    name=$(basename "$f" .lisp)
    echo "=== $name ==="
    python3 src/main.py "$f" -o "output/${name}.c"
    gcc -Wall -pthread "output/${name}.c" build/lisp.o -I. -o "output/bin/${name}"
    ./output/bin/"$name"
done