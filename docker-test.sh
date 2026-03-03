docker build -t lisp-machine-stage1 .
docker run --rm -v ${PWD}/tests:/app/tests lisp-machine-stage1