FROM --platform=linux/amd64 gcc:latest

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
    echo "=== GC Tests ===" && \
    gcc -Wall -pthread tests/test_gc.c build/lisp.o -I. -o build/test_gc && \
    ./build/test_gc && \
    echo "" && \
    echo "=== Python parser tests ===" && \
    python3 -m pytest tests/test_parser.py -v && \
    echo "" && \
    echo "=== End-to-End tests ===" && \
    mkdir -p output && \
    python3 tests/run_e2e_tests.py && \
    echo "" && \
    echo "=== Standard library (lib/) ===" && \
    echo "" && \
    echo "--- library.lisp ---" && \
    echo "Lisp source:" && \
    cat lib/library.lisp && \
    echo "" && \
    echo "Generated C:" && \
    mkdir -p output/library && \
    python3 src/main.py lib/library.lisp -o output/library/library.c && \
    cat output/library/library.c && \
    echo "" && \
    echo "=== All tests passed ==="; \
} 2>&1 | tee tests/test_results.txt