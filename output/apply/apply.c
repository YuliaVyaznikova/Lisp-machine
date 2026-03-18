#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */



int main(int argc, char** argv) {
    lisp_print(lisp_apply(lisp_make_closure(lisp_add_wrapper, NULL), lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), NULL)))));
    lisp_print(lisp_apply(lisp_make_closure(lisp_mul_wrapper, NULL), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), NULL)))));
    return 0;
}