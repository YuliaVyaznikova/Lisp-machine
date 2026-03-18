#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */



int main(int argc, char** argv) {
    lisp_print(lisp_add(lisp_add(lisp_make_int(1), lisp_make_int(2)), lisp_make_int(3)));
    lisp_print(lisp_mul(lisp_make_int(6), lisp_make_int(7)));
    return 0;
}