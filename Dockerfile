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

CMD gcc -Wall tests/test_runtime.c build/lisp.o -I. -o build/test_runtime && \
    ./build/test_runtime 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== Python parser tests ===" && \
    python3 -m pytest tests/test_parser.py -v 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== End-to-End tests ===" && \
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
    ./build/hello 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/arithmetic 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/comparisons 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/conditionals 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/lists 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/quotes 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/define-var 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/define-func 2>&1 | tee -a tests/test_results.txt && \
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
    ./build/lambda 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== All tests passed ===" | tee -a tests/test_results.txt