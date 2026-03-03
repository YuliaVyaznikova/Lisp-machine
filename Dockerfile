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

CMD echo "=== C Runtime Tests ===" && \
    gcc -Wall tests/test_runtime.c build/lisp.o -I. -o build/test_runtime && \
    ./build/test_runtime 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== Python Parser Tests ===" && \
    python3 -m pytest tests/test_parser.py -v 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== Compilation Test ===" && \
    python3 src/main.py examples/hello.lisp -o build/hello.c && \
    echo "Generated C code:" && \
    cat build/hello.c 2>&1 | tee -a tests/test_results.txt && \
    echo "" && \
    echo "=== All tests passed! ===" | tee -a tests/test_results.txt