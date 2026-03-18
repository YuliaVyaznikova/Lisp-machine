#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */



int main(int argc, char** argv) {
    LispValue* x = NULL;
    x = lisp_make_int(42);
    lisp_print(x);
    return 0;
}