FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install pytest --break-system-packages

COPY . /app/

RUN mkdir -p build && \
    gcc -Wall -Wextra -std=c11 -c runtime/lisp.c -o build/lisp.o

CMD { \
    echo "=== Runtime Tests ===" && \
    gcc -Wall tests/test_runtime.c build/lisp.o -I. -o build/test_runtime && \
    ./build/test_runtime && \
    echo "" && \
    echo "=== Python parser tests ===" && \
    python3 -m pytest tests/test_parser.py -v && \
    echo "" && \
    echo "=== End-to-End tests (examples/) ===" && \
    echo "" && \
    echo "--- hello.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/hello.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/hello.lisp -o build/hello.c && \
    cat build/hello.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/hello.c build/lisp.o -I. -o build/hello && \
    ./build/hello && \
    echo "" && \
    echo "--- arithmetic.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/arithmetic.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/arithmetic.lisp -o build/arithmetic.c && \
    cat build/arithmetic.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/arithmetic.c build/lisp.o -I. -o build/arithmetic && \
    ./build/arithmetic && \
    echo "" && \
    echo "--- comparisons.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/comparisons.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/comparisons.lisp -o build/comparisons.c && \
    cat build/comparisons.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/comparisons.c build/lisp.o -I. -o build/comparisons && \
    ./build/comparisons && \
    echo "" && \
    echo "--- conditionals.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/conditionals.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/conditionals.lisp -o build/conditionals.c && \
    cat build/conditionals.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/conditionals.c build/lisp.o -I. -o build/conditionals && \
    ./build/conditionals && \
    echo "" && \
    echo "--- lists.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/lists.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/lists.lisp -o build/lists.c && \
    cat build/lists.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/lists.c build/lisp.o -I. -o build/lists && \
    ./build/lists && \
    echo "" && \
    echo "--- quotes.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/quotes.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/quotes.lisp -o build/quotes.c && \
    cat build/quotes.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/quotes.c build/lisp.o -I. -o build/quotes && \
    ./build/quotes && \
    echo "" && \
    echo "--- define-var.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/define-var.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/define-var.lisp -o build/define-var.c && \
    cat build/define-var.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/define-var.c build/lisp.o -I. -o build/define-var && \
    ./build/define-var && \
    echo "" && \
    echo "--- define-func.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/define-func.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/define-func.lisp -o build/define-func.c && \
    cat build/define-func.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/define-func.c build/lisp.o -I. -o build/define-func && \
    ./build/define-func && \
    echo "" && \
    echo "--- lambda.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/lambda.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/lambda.lisp -o build/lambda.c && \
    cat build/lambda.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/lambda.c build/lisp.o -I. -o build/lambda && \
    ./build/lambda && \
    echo "" && \
    echo "--- closure.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/closure.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/closure.lisp -o build/closure.c && \
    cat build/closure.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/closure.c build/lisp.o -I. -o build/closure && \
    ./build/closure && \
    echo "" && \
    echo "--- apply.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/apply.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/apply.lisp -o build/apply.c && \
    cat build/apply.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/apply.c build/lisp.o -I. -o build/apply && \
    ./build/apply && \
    echo "" && \
    echo "--- tco.lisp (Tail Call Optimization) ---" && \
    echo "Lisp source:" && \
    cat examples/tco.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/tco.lisp -o build/tco.c && \
    cat build/tco.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/tco.c build/lisp.o -I. -o build/tco && \
    ./build/tco && \
    echo "" && \
    echo "--- macros.lisp ---" && \
    echo "Lisp source:" && \
    cat examples/macros.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py examples/macros.lisp -o build/macros.c && \
    cat build/macros.c && \
    echo "" && \
    echo "Output:" && \
    gcc build/macros.c build/lisp.o -I. -o build/macros && \
    ./build/macros && \
    echo "" && \
    echo "=== Standard library (lib/) ===" && \
    echo "" && \
    echo "--- library.lisp ---" && \
    echo "Lisp source:" && \
    cat lib/library.lisp && \
    echo "" && \
    echo "Generated C:" && \
    python3 src/main.py lib/library.lisp -o build/library.c && \
    cat build/library.c && \
    echo "" && \
    echo "=== All tests passed ==="; \
} 2>&1 | tee tests/test_results.txt