#!/bin/bash
mkdir -p build
gcc -c runtime/lisp.c -I. -o build/lisp.o

echo "========================================="
echo "Running all Lisp tests"
echo "========================================="

for f in examples/*.lisp; do
    name=$(basename "$f" .lisp)
    mkdir -p "output/${name}"
    echo ""
    echo "=== $name ==="
    
    python3 src/main.py "$f" -o "output/${name}/${name}.c"
    
    gcc -Wall -pthread "output/${name}/${name}.c" build/lisp.o -I. -o "output/${name}/${name}"
    
    ./output/${name}/"$name" 2>&1 | tee "output/${name}/${name}.output"
done

echo ""
echo "========================================="
echo "All tests complete"
echo "========================================="